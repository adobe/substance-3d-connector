/** @file uuid_utils.h
    @brief Contains utility functions for working with UUIDs
    @author Galen Helfter - Allegorithmic
    @date 20190420
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_UUID_UTILS_H
#define _SUBSTANCE_CONNECTOR_DETAILS_UUID_UTILS_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Forward declare uuid struct */
struct _substance_connector_uuid;

/* Performs a comparison of the two uuids, returning -1, 0, or 1 */
int connector_compare_uuid(const struct _substance_connector_uuid *id0,
                      const struct _substance_connector_uuid *id1);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_UUID_UTILS_H */
