/** @file message.c
    @brief Contains basic operations on messages
    @author Galen Helfter - Allegorithmic
    @date 20181127
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/string_utils.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <arpa/inet.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <Winsock2.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

connector_message_t* connector_build_message(unsigned int context,
                                   const substance_connector_uuid_t *type,
                                   const char *message)
{
    connector_message_t *result = NULL;

    if (message != NULL && type != NULL)
    {
        /* Allocate the message and the header together, allowing
         * the header type to be abstracted away */
        result = connector_allocate(sizeof(connector_message_t) +
                               sizeof(connector_message_header_t));

        if (result != NULL)
        {
            memset(result, 0x00, sizeof(connector_message_t) +
                                 sizeof(connector_message_header_t));

            /* Copy the context and the strings passed in into the message */
            result->context = context;
            result->message = connector_strdup(message);
            result->header = (connector_message_header_t*) ((uint8_t*) result +
                                                       sizeof(connector_message_t));

            /* Set the header with the message length */
            result->header->message_length = (uint32_t) strlen(result->message);
            memcpy(&result->header->message_id, type,
                   sizeof(substance_connector_uuid_t));
            result->header->message_length = (uint32_t) strlen(result->message);

            /* Fill out the description to have the right information */
            result->header->description |= CONNECTOR_HEADER_R1;
            result->header->description |= CONNECTOR_MESSAGE_IDENTIFIER;
        }
    }

    return result;
}

void connector_clear_message(connector_message_t *message)
{
    if (message != NULL)
    {
        connector_free(message->message);

        memset(message, 0x00, sizeof(connector_message_t));
    }
}
