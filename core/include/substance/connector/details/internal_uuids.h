/** @file internal_uuids.h
    @brief Contains constants for internal uuid types
    @author Galen Helfter - Adobe
    @date 20190613
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_INTERNAL_UUIDS_H
#define _SUBSTANCE_CONNECTOR_INTERNAL_UUIDS_H

#include <substance/connector/types.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

extern const substance_connector_uuid_t connector_internal_handshake_uuid;

extern const substance_connector_uuid_t connector_internal_connection_closed_uuid;

extern const substance_connector_uuid_t connector_internal_log_uuid;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_INTERNAL_UUIDS_H */
