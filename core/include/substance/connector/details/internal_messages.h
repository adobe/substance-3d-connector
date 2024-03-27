/** @file internal_messages.h
    @brief Contains internal message operations
    @author Galen Helfter - Allegorithmic
    @date 20190206
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_INTERNAL_MESSAGES_H
#define _SUBSTANCE_CONNECTOR_DETAILS_INTERNAL_MESSAGES_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Attempt to call an internal message based on the message type. This will
 * return the error code from the message, including success if it succeeds.
 * If no such message type was found, will return an invalid message error
 * code. */
unsigned int connector_call_internal_message(unsigned int context,
                                        const substance_connector_uuid_t *type,
                                        const char *message);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_INTERNAL_MESSAGES_H */
