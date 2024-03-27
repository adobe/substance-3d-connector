/** @file pathstringdetails.h
    @brief Contains various path string operation defines
    @author Galen Helfter - Adobe
    @date 20190723
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHSTRINGDETAILS_H
#define _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHSTRINGDETAILS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define CONNECTOR_PATH_DELIM '/'
#define CONNECTOR_PATH_SNPRINTF snprintf
#define CONNECTOR_PATH_STRLEN strlen
#define CONNECTOR_PATH_FOPEN fopen
#define CONNECTOR_PATH_STRCMP strcmp
#define CONNECTOR_PATH_ATOI atoi
#define CONNECTOR_PATH_STRRCHR strrchr
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define CONNECTOR_PATH_DELIM '\\'
#define CONNECTOR_PATH_SNPRINTF _snwprintf
#define CONNECTOR_PATH_STRLEN wcslen
#define CONNECTOR_PATH_FOPEN _wfopen
#define CONNECTOR_PATH_STRCMP wcscmp
#define CONNECTOR_PATH_ATOI _wtoi
#define CONNECTOR_PATH_STRRCHR wcsrchr
#else
#error "Currently only Windows and Posix string utilities are supported"
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_SYSTEM_PATHSTRINGDETAILS_H */
