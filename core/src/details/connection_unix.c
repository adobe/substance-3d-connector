/** @file connection_unix.c
    @brief Provides connection details for Unix systems
    @author Galen Helfter - Allegorithmic
    @date 20181126
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>

/* C standard library headers */
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* Unix headers */
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/connection_details.h>
#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/network/openconnectionimpl.h>
#include <substance/connector/details/network/readwriteutils.h>
#include <substance/connector/details/system/connectiondirectory.h>

#ifndef   SUBSTANCE_CONNECTOR_SOCK_BACKLOG
#define   SUBSTANCE_CONNECTOR_SOCK_BACKLOG 10
#endif /* SUBSTANCE_CONNECTOR_SOCK_BACKLOG */

/* Set MSG_NOSIGNAL flag for the platforms that support it (Linux and BSD) */
#if defined(SUBSTANCE_CONNECTOR_LINUX) || defined(SUBSTANCE_CONNECTOR_UNIX)
#define CONNECTOR_SOCKET_FLAGS MSG_NOSIGNAL
#else
#define CONNECTOR_SOCKET_FLAGS 0
#endif

static connector_readwrite_size_t send_socket(int sock, const void *buffer,
                                         connector_readwrite_buffersize_t len)
{
    return send(sock, buffer, len, CONNECTOR_SOCKET_FLAGS);
}

static connector_readwrite_size_t read_socket(int fd, void *buffer,
                                         connector_readwrite_buffersize_t len)
{
    return recv(fd, buffer, len, 0);
}

static unsigned int close_fd_connection(connector_context_t *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    int retval = 0;

    if (context != NULL)
    {
        retval = close(context->fd);
        if (retval < 0)
        {
            retcode = SUBSTANCE_CONNECTOR_ERROR;
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

static int accept_socket(connector_context_t *context,
                         struct sockaddr *address,
                         socklen_t len)
{
    return accept((int) context->fd, address, &len);
}

unsigned int connector_open_tcp(connector_context_t *context)
{
    return connector_open_tcp_impl(context);
}

int connector_accept_tcp(connector_context_t *context)
{
    struct sockaddr_in address;

    return accept_socket(context, (struct sockaddr*) &address, sizeof(address));
}

unsigned int connector_connect_tcp(connector_context_t *context)
{
    return connector_connect_tcp_impl(context);
}

unsigned int connector_read_tcp(connector_context_t *context, connector_message_t *message)
{
    return connector_read_message_generic(context, message, &read_socket);
}

unsigned int connector_write_tcp(connector_context_t *context, connector_message_t *message)
{
    return connector_send_message_generic(context, message, &send_socket);
}

unsigned int connector_close_tcp(connector_context_t *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Only delete the port file from an open context, which is
     * owned by this instance */
    if (context->configuration & SUBSTANCE_CONNECTOR_CONN_OPEN)
    {
        connector_remove_open_tcp_port(context->port);
    }

    retcode = close_fd_connection(context);

    return retcode;
}

unsigned int connector_open_unix(connector_context_t *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    const char *filepath = NULL;
    struct sockaddr_un address;
    size_t pathlength = 0u;

    int sock = 0;
    int result = 0;
    size_t addr_length = 0u;

    unsigned int local_result = SUBSTANCE_CONNECTOR_ERROR;

    if (context->connection_data == NULL)
    {
        local_result = connector_ensure_default_unix_directory();

        context->connection_data = connector_build_default_unix_path();
    }

    if (context == NULL || local_result != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }
    else
    {
        /* Unix systems - such as MacOS */
        filepath = context->connection_data;
        pathlength = strlen(filepath);

        /* Check for a path being too long to store in a sockaddr_un structure
         * for this platform. On Linux, this should be of size 108, and on
         * MacOS/OpenBSD/FreeBSD of size 104, for reference. */
        if (pathlength + 1 >= sizeof(address.sun_path))
        {
            retcode = SUBSTANCE_CONNECTOR_INVALID;
        }
        else
        {
            sock = socket(AF_UNIX, SOCK_STREAM, 0);
            address.sun_family = AF_UNIX;
            strncpy(address.sun_path, filepath, sizeof(address.sun_path) - 1u);

            if (sock < 0)
            {
                retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
            }
            else
            {
                /* Delete the socket identifier if it exists */
                unlink(filepath);

                /* Acquire platform-specific length to pass to bind */
#if defined(SUBSTANCE_CONNECTOR_LINUX)
                addr_length = sizeof(address);
#elif defined(SUBSTANCE_CONNECTOR_MACOS)
                addr_length = SUN_LEN(&address);
#else
                addr_length = sizeof(address.sun_family)
                              + strlen(address.sun_path);
#endif

                result = bind(sock, (struct sockaddr*) &address, addr_length);

                if (result < 0)
                {
                    retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
                }
                else
                {
                    result = listen(sock, SUBSTANCE_CONNECTOR_SOCK_BACKLOG);

                    if (result < 0)
                    {
                        unlink(filepath);
                        retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
                    }
                    else
                    {
                        /* On success, assign the new file descriptor to the
                         * context. */
                        context->fd = sock;
                        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
                    }
                }

                /* On failure, close the socket */
                if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
                {
                    close(sock);
                }
            }
        }
    }


    return retcode;
}

unsigned int connector_connect_unix(connector_context_t *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    const char *filepath = NULL;
    struct sockaddr_un address;
    size_t pathlength = 0u;
    int sock = 0;
    int result = 0;
    size_t addr_length = 0u;

    if (context == NULL)
    {
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }
    else if (context->connection_data == NULL)
    {
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }
    else
    {
        /* Continue with the attempted connection */
        filepath = context->connection_data;
        pathlength = strlen(filepath);

        if (pathlength + 1 >= sizeof(address.sun_path))
        {
            retcode = SUBSTANCE_CONNECTOR_INVALID;
        }
        else
        {
            sock = socket(AF_UNIX, SOCK_STREAM, 0);

            /* strncpy should be available on Unix and will not truncate the
             * null byte, as the previous length check tested for it. */
            strncpy(address.sun_path, filepath, sizeof(address.sun_path) - 1u);
            address.sun_family = AF_UNIX;

            if (sock < 0)
            {
                retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
            }
            else
            {
                /* Acquire platform-specific length to pass to bind */
#if defined(SUBSTANCE_CONNECTOR_LINUX)
                addr_length = sizeof(address);
#elif defined(SUBSTANCE_CONNECTOR_MACOS)
                addr_length = SUN_LEN(&address);
#else
                addr_length = sizeof(address.sun_family)
                              + strlen(address.sun_path);
#endif

                result = connect(sock, (const struct sockaddr *) &address,
                                 addr_length);

                /* Close the socket on failure to connect and set the proper
                 * errorcode */
                if (result < 0)
                {
                    close(sock);
                    retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
                }
                else
                {
                    context->fd = sock;
                    retcode = SUBSTANCE_CONNECTOR_SUCCESS;
                }
            }
        }
    }

    return retcode;
}

int connector_accept_unix(connector_context_t *context)
{
    struct sockaddr_un address;

    return accept_socket(context, (struct sockaddr*) &address, sizeof(address));
}

unsigned int connector_read_unix(connector_context_t *context, connector_message_t *message)
{
    return connector_read_message_generic(context, message, read_socket);
}

unsigned int connector_write_unix(connector_context_t *context, connector_message_t *message)
{
    return connector_send_message_generic(context, message, &send_socket);
}

unsigned int connector_close_unix(connector_context_t *context)
{
    return close_fd_connection(context);
}
