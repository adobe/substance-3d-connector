/** @file openconnectionimpl.c
    @brief Contains functions for opening a socket connection
    @author Galen Helfter - Adobe
    @date 20190627
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/network/openconnectionimpl.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#define CONNECTOR_SOCKET_TYPE int
#define CONNECTOR_CLOSE_SOCKET close
#define CONNECTOR_ADDRESS_LENGTH_TYPE socklen_t
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#define CONNECTOR_SOCKET_TYPE SOCKET
#define CONNECTOR_CLOSE_SOCKET closesocket
#define CONNECTOR_ADDRESS_LENGTH_TYPE int
#endif

#include <string.h>
#include <stdint.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/system/connectiondirectory.h>

#ifndef   SUBSTANCE_CONNECTOR_SOCK_BACKLOG
#define   SUBSTANCE_CONNECTOR_SOCK_BACKLOG 10
#endif /* SUBSTANCE_CONNECTOR_SOCK_BACKLOG */

static void set_default_fd(CONNECTOR_SOCKET_TYPE fd)
{
    /* Removing sigpipe from a file descriptor is not available on
     * all Unix platforms. */
#if defined(SUBSTANCE_CONNECTOR_MACOS)
    fcntl(fd, F_SETNOSIGPIPE, 1);
#endif
}

unsigned int connector_open_tcp_impl(struct _connector_context *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    struct sockaddr_in address;
    uint16_t port = 0u;
    CONNECTOR_SOCKET_TYPE sock = 0;
    CONNECTOR_ADDRESS_LENGTH_TYPE address_len = 0;
    int result = 0;
    unsigned int library_result = SUBSTANCE_CONNECTOR_ERROR;

    library_result = connector_ensure_default_tcp_directory();

    if (context != NULL && library_result == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        port = (uint16_t) context->port;
        sock = socket(PF_INET, SOCK_STREAM, 0);

        /* Set up socket address structure - ensure everything in network
         * byte order. Standard address. */
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

        if (sock < 0)
        {
            /* Failed to open the socket */
            retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
        }
        else
        {
            result = bind(sock, (struct sockaddr*) &address, sizeof(address));

            if (result < 0)
            {
                /* Failed to bind the socket - set the error code
                 * appropriately. Might check errno for the type of error,
                 * which EADDRINUSE might be the only one we care about. */
                retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
            }
            else
            {
                /* Reacquire the port in case zero was passed in to acquire
                 * an arbitrary socket */
                address_len = sizeof(address);
                result = getsockname(sock, (struct sockaddr*) &address,
                                     &address_len);

                context->port = (uint16_t) ntohs(address.sin_port);

                /* Set to have no sigpipe on supported platforms */
                set_default_fd(sock);

                result = listen(sock, SUBSTANCE_CONNECTOR_SOCK_BACKLOG);
                if (result < 0)
                {
                    retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
                }
                else
                {
                    /* Commit the new file to the tcp socket directory to
                     * allow for automatic connections */
                    connector_commit_open_tcp_port((unsigned int) context->port);

                    context->fd = (size_t) sock;
                    retcode = SUBSTANCE_CONNECTOR_SUCCESS;
                }
            }

            if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
            {
                CONNECTOR_CLOSE_SOCKET(sock);
            }
        }
    }

    return retcode;
}

unsigned int connector_connect_tcp_impl(struct _connector_context *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    struct sockaddr_in address;
    uint16_t port = 0u;
    CONNECTOR_SOCKET_TYPE sock = 0;
    int result = 0;

    if (context != NULL)
    {
        /* Acquire the port from the context */
        port = (uint16_t) context->port;

        /* Clear sockaddr structure */
        memset(&address, 0x00, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
        }
        else
        {
            result = connect(sock, (const struct sockaddr *) &address,
                             sizeof(address));

            if (result < 0)
            {
                /* Failed to connect, clean up appropriately */
                CONNECTOR_CLOSE_SOCKET(sock);
                retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
            }
            else
            {
                context->fd = sock;
                retcode = SUBSTANCE_CONNECTOR_SUCCESS;
            }
        }
    }

    return retcode;
}
