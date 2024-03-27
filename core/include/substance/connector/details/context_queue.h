/** @file context_queue.h
    @brief Contains all registered contexts and the respective operations on
           them
    @author Galen Helfter - Allegorithmic
    @date 20181103
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_DETAILS_CONTEXT_QUEUE_H
#define _SUBSTANCE_CONNECTOR_DETAILS_CONTEXT_QUEUE_H

#include <substance/connector/details/message.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#ifndef SUBSTANCE_CONNECTOR_CONTEXT_COUNT
#define SUBSTANCE_CONNECTOR_CONTEXT_COUNT 32u
#endif /* SUBSTANCE_CONNECTOR_CONTEXT_COUNT */

/* Structure containing the information for a new, open context. This
 * includes the port, any connection data and the configuration for
 * what the communication type will be. */
typedef struct _connector_context_desc
{
    uint64_t configuration;  /* Connection type */
    void *connection_data;   /* Pointer to any connection information */
    uint32_t port;           /* Port for the connection types that use it */
} connector_context_desc_t;

/* Helper function to open a TCP connection, setting up the context description
 * with defaults for that. */
unsigned int connector_context_open_tcp(unsigned int port, unsigned int *identifier);

/* Helper function to open a Unix socket connection, setting up the context
 * description with defaults for that. */
unsigned int connector_context_open_unix(const char *filepath,
                                    unsigned int *identifier);

/* Opens a new context of the specific connection type. Returns a standard
 * error code, while also returning the new context identifier through
 * the identifier pointer. */
unsigned int connector_context_open(const connector_context_desc_t *context_desc,
                               unsigned int *identifier);

/* Helper function to connect to a TCP socket, setting up the context
 * description with the defaults for that. */
unsigned int connector_context_connect_tcp(unsigned int port,
                                      unsigned int *identifier);

/* Helper function to connect to a Unix domain socket, setting up the context
 * description with the defaults for that. */
unsigned int connector_context_connect_unix(const char *filepath,
                                       unsigned int *identifier);

/* Creates a new context, attempting to connect to another context in another
 * instance of Connector. Returns a standard error code, on success returning the
 * new context through the identifier pointer. */
unsigned int connector_context_connect(const connector_context_desc_t *context_desc,
                                  unsigned int *identifier);

/* Attempts to close the specified context, closing any connections that are
 * bound to it and relinquishing any resources from it. It will notify any
 * threads that the context is invalid. */
unsigned int connector_context_close(unsigned int context);

/* Performs a write operation on the given context, writing the given message
 * to the output connection. */
unsigned int connector_context_write(unsigned int context, connector_message_t *message);

/* If the handshake has not been sent, sends it on the given context and
 * sets the state appropriately */
unsigned int connector_context_write_handshake(unsigned int context);

/* Performs a read operation on the given context, storing any successful
 * results into the message passed in. */
unsigned int connector_context_read(unsigned int context, connector_message_t *message);

/* Runs an accept call on the given context, attempting to resolve an incoming
 * connection. This will return a standard error code based on the success of
 * the operation. It will also register the new connection as a context in the
 * context system, returning on success the context identifier through the
 * identifier pointer. */
unsigned int connector_context_accept(unsigned int context, unsigned int *identifier);

/* Initializes the context system by populating the appropriate data
 * structures and performing any other setup operations. */
unsigned int connector_init_context_subsystem(void);

/* Shuts down all contexts and cleans up any resources used by the context
 * system. */
unsigned int connector_shutdown_context_subsystem(void);

/* Returns the connection state of the given context. An invalid context will
 * return UINT_MAX. */
unsigned int connector_context_state(unsigned int context);

/* Returns the communication type of the given context. An invalid context
 * will return UINT_MAX. */
unsigned int connector_context_type(unsigned int context);

/* Returns the port for a given context. Only valid if the type of connection
 * is a TCP connection, as there may be no concept of port in other connection
 * types. */
unsigned int connector_context_port(unsigned int context);

/* Returns the file descriptor or socket ID associated with the context.
 * An invalid context will return -1. */
int connector_context_get_fd(unsigned int context);

/* Finalize the shutdown of a context, to only be called from a read thread
 * that has ownership of the given context. This will be called by the read
 * thread after it has acknowledged that a context should be closed. Returns
 * an errorcode representing success or failure. */
unsigned int connector_context_shutdown_from_read_thread(unsigned int context);

/* Set the application name for the context, after receiving a handshake
 * message. Returns an errorcode representing success or failure. */
unsigned connector_context_set_application_name(unsigned int context,
                                           const char *application_name);

/* Return the context name, or NULL if it is not found or context is out
 * of bounds */
const char* connector_context_get_application_name(unsigned int context);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_DETAILS_CONTEXT_QUEUE_H */
