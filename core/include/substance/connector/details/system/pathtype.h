/** @file pathtype.h
    @brief Contains the system-specific path character type
    @author Galen Helfter - Adobe
    @date 20190719
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHTYPE_H
#define _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHTYPE_H

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#if defined(SUBSTANCE_CONNECTOR_WIN32)
#include <wchar.h>
typedef wchar_t connector_pathchar_t;
#else
typedef char connector_pathchar_t;
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHTYPE_H */
