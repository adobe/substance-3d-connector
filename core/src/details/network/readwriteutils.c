/** @file readwriteutils.c
    @brief Contains utility functions for reading from connections
    @author Galen Helfter - Adobe
    @date 20190626
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/network/readwriteutils.h>

#include <string.h>
#include <stdint.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_header.h>

static unsigned int read_message_data(connector_context_t *context,
                                      connector_message_t *message,
                                      connector_recv_fp read_msg_fn)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    connector_readwrite_size_t result = 0;
    size_t message_length = message->header->message_length + 1u;

    uint8_t *message_buffer = connector_allocate(message_length);

    memset(message_buffer, 0x00, message_length);

    if (message_buffer != NULL)
    {
        result = read_msg_fn((int) context->fd, message_buffer,
                             (connector_readwrite_buffersize_t) message_length - 1u);

        if (result < 0)
        {
            retcode = SUBSTANCE_CONNECTOR_READ_FAIL;
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* Transfer ownership of the buffers to the message structure */
        message->message = (char *) message_buffer;
        message_buffer = NULL;
    }
    else
    {
        connector_free(message_buffer);
        message_buffer = NULL;
    }

    return retcode;
}

unsigned int connector_read_message_generic(struct _connector_context *context,
                                       struct _connector_message *message,
                                       connector_recv_fp read_msg_fn)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_message_header_t header;
    connector_readwrite_size_t result = 0;

    if (message != NULL && context != NULL)
    {
        memset(&header, 0x00, sizeof(header));

        /* Read the header in */
        result = read_msg_fn((int) context->fd, &header, sizeof(header));

        if (result == sizeof(header))
        {
            connector_ntohheader(message->header, &header);

            if (CONNECTOR_IDENTIFY_MESSAGE(message->header->description))
            {
                retcode = read_message_data(context, message, read_msg_fn);
            }
            else
            {
                retcode = SUBSTANCE_CONNECTOR_READ_FAIL;
            }
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_READ_FAIL;
        }
    }

    return retcode;
}

unsigned int connector_send_message_generic(const struct _connector_context *context,
                                       const struct _connector_message *message,
                                       connector_send_fp send_msg_fn)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    int fd = 0;
    connector_readwrite_size_t result = 0;
    uint8_t *buffer = NULL;
    size_t buffersize = 0u;
    size_t buffer_index = 0u;

    if (message != NULL && context != NULL)
    {
        fd = (int) context->fd;

        /* Create a new buffer, with the header at the beginning and the
         * payload at the end */
        buffersize = sizeof(connector_message_header_t)
                     + message->header->message_length;
        buffer = connector_allocate(buffersize);

        /* Pack the header, the message type, and the content payload into the
         * buffer before sending */
        connector_htonheader((connector_message_header_t *) buffer, message->header);

        /* Offset by header and copy the message in */
        buffer_index = sizeof(connector_message_header_t);

        memcpy(buffer + buffer_index, message->message,
               message->header->message_length);

        /* Write the payload out in network-byte order */
        result = send_msg_fn(fd, buffer,
                             (connector_readwrite_buffersize_t) buffersize);
        if (result < 0)
        {
            retcode = SUBSTANCE_CONNECTOR_CONN_FAIL;
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }

        connector_free(buffer);
    }

    return retcode;
}
