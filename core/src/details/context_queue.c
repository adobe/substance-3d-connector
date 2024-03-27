/** @file context_queue.c
    @brief Contains all registered contexts and the respective operations
           for working with contexts.
    @author Galen Helfter - Allegorithmic
    @date 20181103
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/communication.h>
#include <substance/connector/details/configuration.h>
#include <substance/connector/details/connection.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/internal_uuids.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/string_utils.h>
#include <substance/connector/details/uint_queue.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <arpa/inet.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <Winsock2.h>
#endif

#define SUBSTANCE_CONNECTOR_COMM_USED (SUBSTANCE_CONNECTOR_COMM_FIFO + 1)

static connector_context_t context_list[SUBSTANCE_CONNECTOR_CONTEXT_COUNT];

static connector_uint_queue_t *free_contexts = NULL;

static void clear_context_struct(connector_context_t *context)
{
    connector_free(context->application_name);
    connector_free(context->connection_data);

    memset(context, 0x00, sizeof(*context));
}

/* Appends the given context to the available queue, and then flags the read
 * threads to check for an available context */
static unsigned int append_available(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    retcode = connector_available_queue_push(context);

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        retcode = connector_flag_read();
    }

    return retcode;
}

static unsigned int context_op_generic(unsigned int context, context_op_fp op)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_context_t *connector_context = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        connector_context = context_list + context;

        retcode = op(connector_context);
    }

    return retcode;
}

static unsigned int context_message_op_generic(unsigned int context,
                                               connector_message_t *message,
                                               message_op_fp op)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_context_t *connector_context = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT && message != NULL)
    {
        connector_context = context_list + context;

        if ((connector_context->configuration & SUBSTANCE_CONNECTOR_CONN_MASK) ==
            SUBSTANCE_CONNECTOR_CONN_CONNECTED)
        {
            retcode = op(connector_context, message);
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_INVALID;
        }
    }

    return retcode;
}

static unsigned int context_configuration(unsigned int context,
                                          unsigned int comm_mask)
{
    unsigned int config = UINT_MAX;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        config = context_list[context].configuration & comm_mask;
    }

    return config;
}

unsigned int connector_context_open_tcp(unsigned int port, unsigned int *identifier)
{
    connector_context_desc_t context_desc;

    memset(&context_desc, 0x00, sizeof(context_desc));

    context_desc.configuration = SUBSTANCE_CONNECTOR_COMM_TCP;
    context_desc.port = (uint32_t) port;

    return connector_context_open(&context_desc, identifier);
}

unsigned int connector_context_open_unix(const char *filepath,
                                    unsigned int *identifier)
{
    connector_context_desc_t context_desc;

    memset(&context_desc, 0x00, sizeof(context_desc));

    context_desc.configuration = SUBSTANCE_CONNECTOR_COMM_UNIX;

    /* Filepath will not be owned by the context description */
    context_desc.connection_data = (void *) filepath;

    return connector_context_open(&context_desc, identifier);
}

unsigned int connector_context_open(const connector_context_desc_t *context_desc,
                               unsigned int *identifier)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int context = 0u;
    uint64_t comm_type = 0u;
    connector_context_t *context_struct = NULL;

    if (context_desc != NULL && identifier != NULL)
    {
        comm_type = context_desc->configuration & SUBSTANCE_CONNECTOR_COMM_MASK;
        retcode = connector_uint_queue_pop(free_contexts, &context);

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            context_struct = (context_list + context);

            memset(context_struct, 0x00, sizeof(connector_context_t));

            /* Fill out the internal context structure */
            context_struct->configuration = comm_type;
            context_struct->configuration |= SUBSTANCE_CONNECTOR_CONN_OPEN;
            context_struct->connection_data =
                connector_strdup(context_desc->connection_data);
            context_struct->port = context_desc->port;

            retcode = context_op_generic(context, connector_open_connection);

            if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
            {
                /* Handle error - return context */
                memset(context_struct, 0x00, sizeof(connector_context_t));
                connector_uint_queue_push(free_contexts, context);
            }
            else
            {
                /* Return the context identifier and add the context to the
                 * available queue */
                *identifier = context;
                append_available(context);
            }
        }
    }

    return retcode;
}

unsigned int connector_context_connect_tcp(unsigned int port,
                                      unsigned int *identifier)
{
    connector_context_desc_t context_desc;
    memset(&context_desc, 0x00, sizeof(context_desc));
    context_desc.configuration = SUBSTANCE_CONNECTOR_COMM_TCP;
    context_desc.port = port;

    return connector_context_connect(&context_desc, identifier);
}

unsigned int connector_context_connect_unix(const char *filepath,
                                       unsigned int *identifier)
{
    connector_context_desc_t context_desc;
    memset(&context_desc, 0x00, sizeof(context_desc));
    context_desc.configuration = SUBSTANCE_CONNECTOR_COMM_UNIX;

    /* The description does not retain ownership of the filepath pointer */
    context_desc.connection_data = (void *) filepath;

    return connector_context_connect(&context_desc, identifier);
}

unsigned int connector_context_connect(const connector_context_desc_t *context_desc,
                                  unsigned int *identifier)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int context = 0u;
    uint64_t comm_type = 0u;
    connector_context_t *context_struct = NULL;

    if (context_desc != NULL && identifier != NULL)
    {
        comm_type = context_desc->configuration & SUBSTANCE_CONNECTOR_COMM_MASK;
        connector_uint_queue_pop(free_contexts, &context);

        context_struct = (context_list + context);

        memset(context_struct, 0x00, sizeof(connector_context_t));

        /* Fill out the internal context structure */
        context_struct->configuration = comm_type;
        context_struct->configuration |= SUBSTANCE_CONNECTOR_CONN_OPEN;
        context_struct->connection_data = context_desc->connection_data;
        context_struct->port = context_desc->port;

        retcode = context_op_generic(context, connector_bridge_connection);

        if (retcode != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            /* Handle error - Return the context to the queue */
            connector_uint_queue_push(free_contexts, context);
        }
        else
        {
            context_struct->configuration &= ~SUBSTANCE_CONNECTOR_CONN_MASK;
            context_struct->configuration |= SUBSTANCE_CONNECTOR_CONN_CONNECTED;
            context_struct->connection_data = NULL;
            *identifier = context;
            append_available(context);
        }
    }

    return retcode;
}

unsigned int connector_context_close(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    uint64_t connection_state = 0u;
    connector_context_t *context_struct = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        context_struct = (context_list + context);

        connection_state = (context_struct->configuration
                            & SUBSTANCE_CONNECTOR_CONN_MASK);

        if ((connection_state & SUBSTANCE_CONNECTOR_CONN_OPEN)
            || (connection_state & SUBSTANCE_CONNECTOR_CONN_CONNECTED))
        {
            /* Assign the context state to also be shutdown */
            context_struct->configuration |= SUBSTANCE_CONNECTOR_CONN_SHUTDOWN;

            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

unsigned int connector_context_accept(unsigned int context, unsigned int *identifier)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int context_id = 0u;
    connector_context_t *context_struct = NULL;
    int fd = -1;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT
        && identifier != NULL
        && connector_context_state(context) == SUBSTANCE_CONNECTOR_CONN_OPEN)
    {
        fd = connector_accept_connection((context_list + context));

        if (fd >= 0)
        {
            /* Acquire a new context for the incoming connection */
            connector_uint_queue_pop(free_contexts, &context_id);

            context_struct = (context_list + context_id);

            /* Fill out the internal context structure */
            context_struct->configuration = connector_context_type(context);
            context_struct->configuration |= SUBSTANCE_CONNECTOR_CONN_CONNECTED;
            context_struct->fd = fd;

            /* Commit the return of the context identifier */
            *identifier = context_id;
            append_available(context_id);
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_OPEN_FAIL;
        }
    }
    else
    {
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }

    return retcode;
}

unsigned int connector_return_context(unsigned int context)
{
    return connector_uint_queue_push(free_contexts, context);
}

unsigned int connector_context_write(unsigned int context, connector_message_t *message)
{
    return context_message_op_generic(context, message, connector_write_connection);
}

unsigned int connector_context_write_handshake(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Create the message with the proper handshake as internal -
     * specify the context in the message buffer as well as the
     * application name length. This will get sent back in the
     * other end's handshake message */
    connector_message_t *message = NULL;
    connector_context_t *context_struct = NULL;
    const char *application_name = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        context_struct = (context_list + context);

        if ((context_struct->configuration & SUBSTANCE_CONNECTOR_HANDSHAKE_MASK) ==
            SUBSTANCE_CONNECTOR_HANDSHAKE_SENT)
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
        else if ((context_struct->configuration & SUBSTANCE_CONNECTOR_CONN_MASK)
                 == SUBSTANCE_CONNECTOR_CONN_CONNECTED)
        {
            /* Message size will be the length of the application name */
            application_name = connector_get_application_name();

            /* Pack the application name and create the message structure */
            message = connector_build_message(context, &connector_internal_handshake_uuid,
                                         application_name);

            if (message != NULL)
            {
                /* Set the message header as an internal message, it is to be
                * processed internally and not forwarded to the user level. */
                message->header->description |= CONNECTOR_INTERNAL_IDENTIFIER;

                /* Write the message out to the context */
                retcode = context_message_op_generic(context, message,
                                                     connector_write_connection);
                if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
                {
                    context_struct->configuration
                        |= SUBSTANCE_CONNECTOR_HANDSHAKE_SENT;
                }
            }

            /* Free any resources allocated for this operation */
            connector_clear_message(message);
            connector_free(message);
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

unsigned int connector_context_read(unsigned int context, connector_message_t *message)
{
    return context_message_op_generic(context, message, connector_read_connection);
}

unsigned int connector_init_context_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    unsigned int i = 0u;

    /* If on Windows, start up Winsock2 */
#if defined(SUBSTANCE_CONNECTOR_WIN32) && !defined(SUBSTANCE_CONNECTOR_NO_INIT_WINSOCK)
    WORD winsock_version;
    WSADATA winsock_data;
    int result = 0;

    winsock_version = MAKEWORD(2, 2);
    result = WSAStartup(winsock_version, &winsock_data);

    if (result != 0)
    {
        retcode = SUBSTANCE_CONNECTOR_ERROR;
    }
#endif

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        memset(context_list, 0x00, sizeof(context_list));

        free_contexts = connector_uint_queue_create(SUBSTANCE_CONNECTOR_CONTEXT_COUNT);

        if (free_contexts != NULL)
        {
            for (i = 0u; i < SUBSTANCE_CONNECTOR_CONTEXT_COUNT; ++i)
            {
                connector_uint_queue_push(free_contexts, i);
            }
        }
        else
        {
            retcode = SUBSTANCE_CONNECTOR_ERROR;
        }
    }

    return retcode;
}

unsigned int connector_shutdown_context_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Destroy the context queue */
    retcode = connector_uint_queue_destroy(free_contexts);
    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        free_contexts = NULL;
    }

    return retcode;
}

unsigned int connector_context_state(unsigned int context)
{
    return context_configuration(context, SUBSTANCE_CONNECTOR_CONN_MASK);
}

unsigned int connector_context_type(unsigned int context)
{
    return context_configuration(context, SUBSTANCE_CONNECTOR_COMM_MASK);
}

unsigned int connector_context_port(unsigned int context)
{
    return (unsigned int) context_list[context].port;
}

int connector_context_get_fd(unsigned int context)
{
    int fd = -1;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        fd = (int) context_list[context].fd;
    }

    return fd;
}

unsigned int connector_context_shutdown_from_read_thread(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_context_t *context_struct = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        /* Close the socket file descriptor */
        context_struct = (context_list + context);

        retcode = connector_close_connection(context_struct);

        /* Enter the trampoline call from the read thread here instead
         * of from a dispatch thread. We must ensure that this is called
         * before a connection established call can be done reusing the
         * same context. */
        if (context_struct->application_name != NULL)
        {
            connector_notify_trampolines(context,
                                    &connector_internal_connection_closed_uuid,
                                    context_struct->application_name);
        }

        /* Clear the context structure. This must be done to
         * ensure that state is completely cleared between
         * reuses of the context */
        clear_context_struct(context_struct);

        /* Return identifier to the free context queue */
        connector_uint_queue_push(free_contexts, context);
    }

    return retcode;
}

unsigned int connector_context_set_application_name(unsigned int context,
                                               const char *application_name)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_context_t *context_struct = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT && application_name != NULL)
    {
        context_struct = (context_list + context);

        connector_free(context_struct->application_name);

        context_struct->application_name = connector_strdup(application_name);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

const char* connector_context_get_application_name(unsigned int context)
{
    const char* name = NULL;
    connector_context_t *context_struct = NULL;

    if (context < SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
    {
        context_struct = (context_list + context);

        name = context_struct->application_name;
    }

    return name;
}

