/** @file message.h
    @brief Contains the definitions for working with messages
    @author Galen Helfter - Allegorithmic
    @date 20181103
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_MESSAGE_H
#define _SUBSTANCE_CONNECTOR_MESSAGE_H

#include <stdint.h>
#include <stddef.h>

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef struct _connector_message_header_r1 connector_message_header_t;

typedef struct _connector_message
{
    connector_message_header_t *header; /* Message header */
    char *message;                 /* Buffer containing the message data */
    unsigned int context;          /* Context identifier */
} connector_message_t;

connector_message_t* connector_build_message(unsigned int context,
                                   const substance_connector_uuid_t *type,
                                   const char *message);
                                     

/* Clears a message structure, freeing any internal memory that it holds and
 * zeroing out any values. */
void connector_clear_message(connector_message_t *message);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_MESSAGE_H */
