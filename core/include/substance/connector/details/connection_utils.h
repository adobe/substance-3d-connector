/** @file connection_utils.h
    @brief Contains utilities for all platform connection implementations
    @author Galen Helfter - Allegorithmic
    @date 20181217
    @copyright Allegorithmic. All rights reserved.
 */

#ifndef _SUBSTANCE_CONNECTOR_CONNECTION_UTILS_H
#define _SUBSTANCE_CONNECTOR_CONNECTION_UTILS_H

#include <substance/connector/common.h>
#include <substance/connector/details/message.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <poll.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <winsock2.h>
#endif

/* Enum specifying return codes for a select or polling call */
enum SubstanceConnectorPollError
{
    /* Success */
    SUBSTANCE_CONNECTOR_POLL_ESUCCESS = 0u,
    /* Bad descriptors */
    SUBSTANCE_CONNECTOR_POLL_EBADF    = 1u,
    /* File descriptor structures were not part of the address space */
    SUBSTANCE_CONNECTOR_POLL_EFAULT   = 2u,
    /* Interrupted before any processing */
    SUBSTANCE_CONNECTOR_POLL_EINTR    = 3u,
    /* The network system has failed */
    SUBSTANCE_CONNECTOR_POLL_ENETDOWN = 4u,
    /* No memory to allocate internal structures */
    SUBSTANCE_CONNECTOR_POLL_ENOMEM   = 5u,
    /* Invalid timeout value */
    SUBSTANCE_CONNECTOR_POLL_EINVAL   = 6u,
    /* Unknown error */
    SUBSTANCE_CONNECTOR_POLL_EUNKNOWN = 7u,

    /* Timed out of the polling call */
    SUBSTANCE_CONNECTOR_POLL_TIMEOUT  = 1200u
};

enum SubstanceConnectorPollEvent
{
    /* Inbound data can be retrieved without blocking */
    SUBSTANCE_CONNECTOR_POLLIN  = POLLIN,
    /* An error has occurred with the given file descriptor */
    SUBSTANCE_CONNECTOR_POLLERR = POLLERR,

    /* Only supported using a call to poll instead of select */

    /* The other side has closed the socket */
    SUBSTANCE_CONNECTOR_POLLHUP = POLLHUP
};

/* Define the poll type as a pollfd, so that the structure may be used
 * efficiently in poll calls. May need to add a manual define for the
 * structure as well if we come across a system that does not define
 * the pollfd struct. */
typedef struct pollfd connector_poll_t;

/* Blocks on each context, waiting for events to occur. Will return either
 * one of the error codes in the SubstanceConnectorPollError enum or will return
 * SUBSTANCE_CONNECTOR_POLL_TIMEOUT, indicating that the poll timed out. A return
 * of SUBSTANCE_CONNECTOR_ESUCCESS indicates that the call succeeded, placing the
 * proper events into the poll structure. */
unsigned int connector_poll_contexts(connector_poll_t *contexts,
                                unsigned int context_count);

/* Convert a Substance Connector header from host byte ordering to network
 * byte ordering */
void connector_htonheader(connector_message_header_t *target,
                     const connector_message_header_t *original);

/* Convert a Substance Connector header from network byte ordering to host
 * byte ordering */
void connector_ntohheader(connector_message_header_t *target,
                     const connector_message_header_t *original);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_CONNECTION_UTILS_H */
