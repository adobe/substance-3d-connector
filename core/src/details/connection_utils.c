/** @file connection_utils.c
    @brief Contains utilities for all platform connection implementations
    @author Galen Helfter - Allegorithmic
    @date 20181217
    @copyright Allegorithmic. All rights reserved.
 */

#include <substance/connector/common.h>

#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_queue.h> /* For the context count */
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/message_header.h>

/* Posix network information */
#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <arpa/inet.h> /* Posix/Unix headers */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#define SUBSTANCE_CONNECTOR_SELECT_ERROR   -1
#define SUBSTANCE_CONNECTOR_SELECT_TIMEOUT  0
#define CONNECTOR_NETWORK_ERROR   errno
#define CONNECTOR_POLL            poll
#define CONNECTOR_POLL_EVENTS     (POLLIN | POLLNVAL)
#define CONNECTOR_SELECT_EBADF    EBADF
#define CONNECTOR_SELECT_EFAULT   EFAULT
#define CONNECTOR_SELECT_EINTR    EINTR
#define CONNECTOR_SELECT_ENETDOWN ENETDOWN
#define CONNECTOR_SELECT_ENOMEM   ENOMEM
#define CONNECTOR_SELECT_EINVAL   EINVAL
#define CONNECTOR_SOCKLEN_T       socklen_t
/* Windows network information */
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <Winsock2.h>
#define SUBSTANCE_CONNECTOR_SELECT_ERROR   SOCKET_ERROR
#define SUBSTANCE_CONNECTOR_SELECT_TIMEOUT 0
#define CONNECTOR_NETWORK_ERROR   WSAGetLastError()
#define CONNECTOR_POLL            WSAPoll
#define CONNECTOR_POLL_EVENTS     POLLIN
#define CONNECTOR_SELECT_EBADF    WSAENOTSOCK
#define CONNECTOR_SELECT_EFAULT   WSAEFAULT
#define CONNECTOR_SELECT_EINTR    WSAEINTR
#define CONNECTOR_SELECT_ENETDOWN WSAENETDOWN
#define CONNECTOR_SELECT_ENOMEM   WSA_NOT_ENOUGH_MEMORY
#define CONNECTOR_SELECT_EINVAL   WSAEINVAL
#define CONNECTOR_SOCKLEN_T       int
#endif

#ifndef SUBSTANCE_CONNECTOR_SELECT_S
#define SUBSTANCE_CONNECTOR_SELECT_S 5
#endif

#ifndef SUBSTANCE_CONNECTOR_SELECT_US
#define SUBSTANCE_CONNECTOR_SELECT_US 0
#endif

#ifndef SUBSTANCE_CONNECTOR_POLL_MS
#define SUBSTANCE_CONNECTOR_POLL_MS 50
#endif

/* Force poll for now because select does not work */
#define SUBSTANCE_CONNECTOR_FORCE_POLL 0x01u
#if defined(SUBSTANCE_CONNECTOR_FORCE_SELECT)
#define SUBSTANCE_CONNECTOR_USE_SELECT
#elif defined(SUBSTANCE_CONNECTOR_FORCE_POLL)
#define SUBSTANCE_CONNECTOR_USE_POLL
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_USE_SELECT
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
/* Will be changed when the poll call implementation is introduced */
#define SUBSTANCE_CONNECTOR_USE_POLL
/* #define SUBSTANCE_CONNECTOR_USE_POLL */
/* #define SUBSTANCE_CONNECTOR_USE_SELECT */
#else
#error "Unknown platform - must use either poll or select"
#endif

/* Convert a uuid to network byte ordering */
static void htonuuid(substance_connector_uuid_t *target,
                     const substance_connector_uuid_t *original)
{
    target->elements[0u] = htonl(original->elements[0u]);
    target->elements[1u] = htonl(original->elements[1u]);
    target->elements[2u] = htonl(original->elements[2u]);
    target->elements[3u] = htonl(original->elements[3u]);
}

/* Convert a uuid to host byte ordering */
static void ntohuuid(substance_connector_uuid_t *target,
                     const substance_connector_uuid_t *original)
{
    target->elements[0u] = ntohl(original->elements[0u]);
    target->elements[1u] = ntohl(original->elements[1u]);
    target->elements[2u] = ntohl(original->elements[2u]);
    target->elements[3u] = ntohl(original->elements[3u]);
}

/* Transform system-specific errors into connection util application errors */
static unsigned int convert_network_error(int errorcode)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_POLL_EUNKNOWN;

    switch (errorcode)
    {
        case CONNECTOR_SELECT_EBADF:
            errorcode = SUBSTANCE_CONNECTOR_POLL_EBADF;
            break;
        case CONNECTOR_SELECT_EFAULT:
            errorcode = SUBSTANCE_CONNECTOR_POLL_EFAULT;
            break;
        case CONNECTOR_SELECT_EINTR:
            errorcode = SUBSTANCE_CONNECTOR_POLL_EINTR;
            break;
        case CONNECTOR_SELECT_ENETDOWN:
            errorcode = SUBSTANCE_CONNECTOR_POLL_ENETDOWN;
            break;
        case CONNECTOR_SELECT_ENOMEM:
            errorcode = SUBSTANCE_CONNECTOR_POLL_ENOMEM;
            break;
        case CONNECTOR_SELECT_EINVAL:
            errorcode = SUBSTANCE_CONNECTOR_POLL_EINVAL;
            break;
        default:
            break;
    }

    return retcode;
}

#if defined(SUBSTANCE_CONNECTOR_USE_SELECT)
unsigned int connector_poll_contexts(connector_poll_t *contexts,
                                unsigned int context_count)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_POLL_EUNKNOWN;
    int retval = 0;
    int errorcode = 0;
    unsigned int i = 0u;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    struct timeval timeout;

    timeout.tv_sec = SUBSTANCE_CONNECTOR_SELECT_S;
    timeout.tv_usec = SUBSTANCE_CONNECTOR_SELECT_US;

    if (contexts != NULL && context_count > 0u)
    {
        /* Clear fd sets */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        /* Add the contexts to the read and except sets */
        for (i = 0u; i < context_count; ++i)
        {
            FD_SET(contexts[i].fd, &readfds);
            FD_SET(contexts[i].fd, &exceptfds);
        }

        /* Perform select call on the given file descriptors */
        retval = select((int) context_count, &readfds,
                        &writefds, &exceptfds, &timeout);

        /* Determine return reasoning */
        if (retval == SUBSTANCE_CONNECTOR_SELECT_ERROR)
        {
            /* Acquire the errorcode */
            errorcode = CONNECTOR_NETWORK_ERROR;
            retcode = convert_network_error(errorcode);
        }
        else if (retval == SUBSTANCE_CONNECTOR_SELECT_TIMEOUT)
        {
            retcode = SUBSTANCE_CONNECTOR_POLL_TIMEOUT;
        }
        else
        {
            /* Iterate over all structures and append any flags */
            for (i = 0u; i < context_count; ++i)
            {
                if (FD_ISSET(contexts[i].fd, &readfds))
                {
                    contexts[i].revents |= SUBSTANCE_CONNECTOR_POLLIN;
                }
                if (FD_ISSET(contexts[i].fd, &exceptfds))
                {
                    contexts[i].revents |= SUBSTANCE_CONNECTOR_POLLERR;
                }
            }
        }
    }

    return retcode;
}
#elif defined(SUBSTANCE_CONNECTOR_USE_POLL)
unsigned int connector_poll_contexts(connector_poll_t *contexts,
                                unsigned int context_count)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_POLL_EUNKNOWN;
    int retval = 0;
    int errorcode = 0;

    if (contexts != NULL && context_count > 0u)
    {
        retval = CONNECTOR_POLL(contexts, context_count, SUBSTANCE_CONNECTOR_POLL_MS);

        /* Determine return reasoning - poll on Unix and Windows return the
         * same as a select call */
        if (retval == SUBSTANCE_CONNECTOR_SELECT_ERROR)
        {
            /* Acquire the errorcode */
            errorcode = CONNECTOR_NETWORK_ERROR;
            retcode = convert_network_error(errorcode);
        }
        else if (retval == SUBSTANCE_CONNECTOR_SELECT_TIMEOUT)
        {
            retcode = SUBSTANCE_CONNECTOR_POLL_TIMEOUT;
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_POLL_ESUCCESS;
        }
    }

    return retcode;
}
#endif

void connector_htonheader(connector_message_header_t *target,
                     const connector_message_header_t *original)
{
    target->description = htons(original->description);
    target->message_length = htonl(original->message_length);
    htonuuid(&target->message_id, &original->message_id);
}

void connector_ntohheader(connector_message_header_t *target,
                     const connector_message_header_t *original)
{
    target->description = ntohs(original->description);
    target->message_length = ntohl(original->message_length);
    ntohuuid(&target->message_id, &original->message_id);
}
