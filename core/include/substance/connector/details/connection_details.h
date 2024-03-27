/** @file connection_details.h
    @brief Provides the specifics for connecting on the current platform
    @author Galen Helfter - Allegorithmic
    @date 20181126
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_CONNECTION_DETAILS_H
#define _SUBSTANCE_CONNECTOR_CONNECTION_DETAILS_H

#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/message.h>

/* These function pointer typedefs map to the implementation-specific
 * functions laid out below. */
typedef unsigned int (*connector_open_fp)(connector_context_t *context);
typedef unsigned int (*connector_connect_fp)(connector_context_t *context);
typedef unsigned int (*connector_read_fp)(connector_context_t *context,
                                     connector_message_t *message);
typedef unsigned int (*connector_write_fp)(connector_context_t *context,
                                      connector_message_t *message);
typedef unsigned int (*connector_close_fp)(connector_context_t *context);

/* Accept functions return a file descriptor */
typedef int (*connector_accept_fp)(connector_context_t *context);

unsigned int connector_open_tcp(connector_context_t *context);
unsigned int connector_connect_tcp(connector_context_t *context);
unsigned int connector_read_tcp(connector_context_t *context, connector_message_t *message);
unsigned int connector_write_tcp(connector_context_t *context, connector_message_t *message);
unsigned int connector_close_tcp(connector_context_t *context);

int connector_accept_tcp(connector_context_t *context);

unsigned int connector_open_unix(connector_context_t *context);
unsigned int connector_connect_unix(connector_context_t *context);
unsigned int connector_read_unix(connector_context_t *context, connector_message_t *message);
unsigned int connector_write_unix(connector_context_t *context, connector_message_t *message);
unsigned int connector_close_unix(connector_context_t *context);

int connector_accept_unix(connector_context_t *context);

#endif /* _SUBSTANCE_CONNECTOR_CONNECTION_DETAILS_H */
