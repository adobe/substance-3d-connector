/** @file connection.c
    @brief Initializes the connection to the server on the other application
    @author Galen Helfter - Allegorithmic
    @date 20181019
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>

#include <substance/connector/details/connection_details.h>
#include <substance/connector/details/connection.h>

static unsigned int default_context_operation(connector_context_t *context)
{
    SUBSTANCE_CONNECTOR_UNUSED(context);

    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

static unsigned int default_message_operation(connector_context_t *context,
                                              connector_message_t *message)
{
    SUBSTANCE_CONNECTOR_UNUSED(context);
    SUBSTANCE_CONNECTOR_UNUSED(message);

    return SUBSTANCE_CONNECTOR_UNSUPPORTED;
}

static int default_accept_operation(connector_context_t *context)
{
    SUBSTANCE_CONNECTOR_UNUSED(context);

    return -1;
}

static unsigned int message_operation(connector_context_t *context,
                                      connector_message_t *message,
                                      message_op_fp *op_table)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    uint8_t connection = 0u;

    if (context != NULL && message != NULL)
    {
        /* Acquire the connection type from the description */
        connection = context->configuration & SUBSTANCE_CONNECTOR_COMM_MASK;

        /* Clamp to the maximum connection type */
        connection = connection > SUBSTANCE_CONNECTOR_COMM_MAX ?
            0u : connection;

        retcode = op_table[connection](context, message);
    }

    return retcode;
}

static unsigned int context_operation(connector_context_t *context,
                                      context_op_fp *op_table)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    uint8_t connection = 0u;

    if (context != NULL)
    {
        /* Acquire the connection type from the description */
        connection = context->configuration & SUBSTANCE_CONNECTOR_COMM_MASK;

        /* Clamp to the maximum connection type */
        connection = connection > SUBSTANCE_CONNECTOR_COMM_MAX ?
            0u : connection;

        retcode = op_table[connection](context);
    }

    return retcode;
}

/* Connection operation table */
static connector_open_fp open_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_context_operation,
    connector_open_tcp,
    connector_open_unix
};

static connector_connect_fp connect_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_context_operation,
    connector_connect_tcp,
    connector_connect_unix
};

static connector_read_fp read_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_message_operation,
    connector_read_tcp,
    connector_read_unix
};

static connector_write_fp write_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_message_operation,
    connector_write_tcp,
    connector_write_unix
};

static connector_close_fp close_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_context_operation,
    connector_close_tcp,
    connector_close_unix
};

static connector_accept_fp accept_functions[SUBSTANCE_CONNECTOR_COMM_MAX + 1u] =
{
    default_accept_operation,
    connector_accept_tcp,
    connector_accept_unix
};

unsigned int connector_open_connection(connector_context_t *context)
{
    return context_operation(context, open_functions);
}

unsigned int connector_bridge_connection(connector_context_t *context)
{
    return context_operation(context, connect_functions);
}

unsigned int connector_read_connection(connector_context_t *context,
                                  connector_message_t *message)
{
    return message_operation(context, message, read_functions);
}

unsigned int connector_write_connection(connector_context_t *context,
                                   connector_message_t *message)
{
    return message_operation(context, message, write_functions);
}

unsigned int connector_close_connection(connector_context_t *context)
{
    return context_operation(context, close_functions);
}

int connector_accept_connection(connector_context_t *context)
{
    int fd = -1;
    uint8_t connection = 0u;

    if (context != NULL)
    {
        /* Acquire the connection type from the description */
        connection = context->configuration & SUBSTANCE_CONNECTOR_COMM_MASK;

        /* Clamp to the maximum connection type */
        connection = connection > SUBSTANCE_CONNECTOR_COMM_MAX ?
            SUBSTANCE_CONNECTOR_COMM_MAX : connection;

        fd = accept_functions[connection](context);
    }

    return fd;
}
