/** @file openconnectionimpl.h
    @brief Contains functions for opening a socket connection
    @author Galen Helfter - Adobe
    @date 20190627
    @copyright Adobe. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_OPENCONNECTIONIMPL_H
#define _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_OPENCONNECTIONIMPL_H

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

struct _connector_context;

/* Shared implementation of opening a tcp socket. This will take
 * the context and open a new tcp connection with it. */
unsigned int connector_open_tcp_impl(struct _connector_context *context);

/* Shared implementation of connecting to a tcp socket. This will modify
 * the context to be a valid connection if it successfully connects to
 * the tcp port. */
unsigned int connector_connect_tcp_impl(struct _connector_context *context);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_NETWORK_OPENCONNECTIONIMPL_H */
