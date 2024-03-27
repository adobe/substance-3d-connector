/** @file disconnect_message.h
    @brief Handles error disconnects by propagating a disconnect message up
    @author Galen Helfter - Adobe
    @date 20200806
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DISCONNECT_MESSAGE_H
#define _SUBSTANCE_CONNECTOR_DISCONNECT_MESSAGE_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/** @brief Handle creation and calling of an error disconnect message
    @param context Context to flag as an error disconnect
    @return Errorcode representing success or failure
*/
unsigned int connector_handle_error_disconnect(unsigned int context);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DISCONNECT_MESSAGE_H */

