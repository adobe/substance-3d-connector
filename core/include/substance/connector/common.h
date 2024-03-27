/** @file common.h
    @brief Contains system defines and other common information
    @author Galen Helfter - Allegorithmic
    @date 20181012
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_COMMON_H
#define _SUBSTANCE_CONNECTOR_COMMON_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Common platform defines */
#if defined(__ANDROID__)
#define SUBSTANCE_CONNECTOR_ANDROID 1
#define SUBSTANCE_CONNECTOR_POSIX   1
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define SUBSTANCE_CONNECTOR_LINUX 1
#define SUBSTANCE_CONNECTOR_POSIX 1
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define SUBSTANCE_CONNECTOR_IOS   1
#else
#define SUBSTANCE_CONNECTOR_MACOS 1 /* MacOS keeps a separate flag */
#endif
#define SUBSTANCE_CONNECTOR_POSIX 1
#elif defined(__unix__)
#define SUBSTANCE_CONNECTOR_UNIX  1 /* SUBSTANCE_CONNECTOR_UNIX for BSD/Solaris */
#define SUBSTANCE_CONNECTOR_POSIX 1
#elif defined(_WIN64)
#define SUBSTANCE_CONNECTOR_WIN64 1
#define SUBSTANCE_CONNECTOR_WIN32 1
#elif defined(_WIN32)
#define SUBSTANCE_CONNECTOR_WIN32 1
#endif

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define _POSIX_C_SOURCE 200809L
#ifndef   _DEFAULT_SOURCE
#define   _DEFAULT_SOURCE
#endif /* _DEFAULT_SOURCE */
#endif

#if defined(SUBSTANCE_CONNECTOR_MACOS) || defined(SUBSTANCE_CONNECTOR_IOS)
#ifndef   _DARWIN_C_SOURCE
#define   _DARWIN_C_SOURCE
#endif /* _DARWIN_C_SOURCE */
#endif

/* Dynamic library handle types */
#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define SUBSTANCE_CONNECTOR_HANDLE void*
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_HANDLE HMODULE
#endif /* SUBSTANCE_CONNECTOR_POSIX */

/* External visibility types */
#if defined(SUBSTANCE_CONNECTOR_POSIX) /* This assumes GCC/Clang */
#define SUBSTANCE_CONNECTOR_EXPORT __attribute__((visibility("default")))
#define SUBSTANCE_CONNECTOR_HEADER_EXPORT
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_EXPORT __declspec(dllexport)
#define SUBSTANCE_CONNECTOR_HEADER_EXPORT __declspec(dllexport)
#endif /* SUBSTANCE_CONNECTOR_POSIX */

/* Define boolean mappings */
#define SUBSTANCE_CONNECTOR_FALSE 0
#define SUBSTANCE_CONNECTOR_TRUE  1

#define SUBSTANCE_CONNECTOR_UNUSED(x) (void)(x)

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_COMMON_H */
