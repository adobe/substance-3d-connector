/** @file connection_windows.c
    @brief Provides connection details for Windows
    @author Galen Helfter - Allegorithmic
    @date 20181126
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>

#include <winsock2.h>
#include <winerror.h>
#include <ws2tcpip.h>

#include <stdlib.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/connection.h>
#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/network/openconnectionimpl.h>
#include <substance/connector/details/network/readwriteutils.h>
#include <substance/connector/details/system/connectiondirectory.h>

static connector_readwrite_size_t read_socket(int fd, void *buffer,
                                         connector_readwrite_buffersize_t len)
{
    return recv(fd, buffer, len, 0);
}

static connector_readwrite_size_t send_socket(int fd, const void *buffer,
                                         connector_readwrite_buffersize_t len)
{
    return send(fd, buffer, len, 0);
}

unsigned int connector_open_tcp(connector_context_t *context)
{
    return connector_open_tcp_impl(context);
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
    int retval = 0;
    unsigned int local_retcode = SUBSTANCE_CONNECTOR_SUCCESS;

    /* Only delete the port file from an open context, which
     * is owned by this instance */
    if (context->configuration & SUBSTANCE_CONNECTOR_CONN_OPEN)
    {
        connector_remove_open_tcp_port((unsigned int) context->port);
    }

    retval = closesocket((SOCKET) context->fd);

    if (retval == 0 && local_retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }
    else if (local_retcode != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = local_retcode;
    }

    return retcode;
}

int connector_accept_tcp(connector_context_t *context)
{
    struct sockaddr_in address;
	int length = (int) sizeof(address);

    return (int) accept((SOCKET) context->fd, (struct sockaddr*) &address,
                        &length);
}

unsigned int connector_open_unix(connector_context_t *context)
{
    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

unsigned int connector_connect_unix(connector_context_t *context)
{
    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

int connector_accept_unix(connector_context_t *context)
{
    return -1;
}

unsigned int connector_read_unix(connector_context_t *context, connector_message_t *message)
{
    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

unsigned int connector_write_unix(connector_context_t *context, connector_message_t *message)
{
    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

unsigned int connector_close_unix(connector_context_t *context)
{
    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}
