/** @file readwriteutils.h
    @brief Contains utility functions for reading from connections
    @author Galen Helfter - Adobe
    @date 20190626
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_READWRITEUTILS_H
#define _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_READWRITEUTILS_H

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

struct _connector_context;
struct _connector_message;

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <stddef.h>
#include <sys/types.h>
typedef ssize_t connector_readwrite_size_t;
typedef size_t connector_readwrite_buffersize_t;
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
typedef int connector_readwrite_size_t;
typedef int connector_readwrite_buffersize_t;
#endif

typedef connector_readwrite_size_t (*connector_recv_fp)(int sock,
                                              void *buf,
                                              connector_readwrite_buffersize_t len);

typedef connector_readwrite_size_t (*connector_send_fp)(int sock,
                                              const void *buf,
                                              connector_readwrite_buffersize_t len);

unsigned int connector_read_message_generic(struct _connector_context *context,
                                       struct _connector_message *message,
                                       connector_recv_fp read_msg_fn);

unsigned int connector_send_message_generic(const struct _connector_context *context,
                                       const struct _connector_message *message,
                                       connector_send_fp send_msg_fn);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_READWRITEUTILS_H */
