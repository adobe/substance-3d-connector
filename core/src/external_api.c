/** @file external_api.c
    @brief Defines the externally available API
    @author Galen Helfter - Allegorithmic
    @date 20181015
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/connector.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/communication.h>
#include <substance/connector/details/configuration.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/dispatch.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/state.h>
#include <substance/connector/details/network/autoconnect.h>
#include <substance/connector/details/system/connectiondirectory.h>

#include <stdlib.h>

/* Create function table to bind function pointers */
SUBSTANCE_CONNECTOR_EXPORT
const struct substance_connector_function_table substance_connector_module_table =
{
    &substance_connector_version,
    &substance_connector_init,
    &substance_connector_shutdown,
    &substance_connector_set_allocators,
    &substance_connector_write_message,
    &substance_connector_add_trampoline,
    &substance_connector_remove_trampoline,
    &substance_connector_open_tcp,
    &substance_connector_open_unix,
    &substance_connector_close_context,
    &substance_connector_connect_tcp,
    &substance_connector_connect_unix,
    &substance_connector_broadcast_tcp,
    &substance_connector_broadcast_unix,
    &substance_connector_broadcast_default,
    &substance_connector_open_default_tcp,
    &substance_connector_open_default_unix,
    &substance_connector_open_default
};

SUBSTANCE_CONNECTOR_EXPORT
const char* substance_connector_version(void)
{
    return SUBSTANCE_CONNECTOR_VERSION;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_init(const char *application_name)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    uint32_t initialized = 0u;

    /* Initialize should only get called if the state is currently at a clean
     * shutdown. The difference between a started and completed state ensures
     * that a shutdown call cannot occur during an initialize call. */
    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                 SUBSTANCE_CONNECTOR_STATE_SHUTDOWN,
                                 SUBSTANCE_CONNECTOR_STATE_INIT_STARTED,
                                 initialized);

    if (initialized == SUBSTANCE_CONNECTOR_STATE_SHUTDOWN)
    {
        retcode = connector_set_application_name(application_name);

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_setup_default_tcp_directory();
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_init_context_subsystem();
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_init_message_queue_subsystem();
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_init_trampoline_subsystem();
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_init_dispatch_subsystem(SUBSTANCE_CONNECTOR_FALSE);
        }

        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = connector_init_comm_subsystem();
        }

        /* Set state to initialization being completed. */
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                         SUBSTANCE_CONNECTOR_STATE_INIT_STARTED,
                                         SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED,
                                         initialized);
        }
        else
        {
            /* Internal shutdown error has occurred - lock the state to
             * an internal error state to ensure it is invalid. */
            CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                         SUBSTANCE_CONNECTOR_STATE_INIT_STARTED,
                                         SUBSTANCE_CONNECTOR_STATE_INTERNAL_ERROR,
                                         initialized);
        }
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_shutdown(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int sub_retcode = SUBSTANCE_CONNECTOR_ERROR;
    uint32_t initialized = 0u;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                 SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED,
                                 SUBSTANCE_CONNECTOR_STATE_SHUTDOWN_STARTED,
                                 initialized);

    if (initialized == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_shutdown_context_subsystem();

        /* Shut down the modules in the opposite order that they were
         * initialized in. */
        sub_retcode = connector_shutdown_comm_subsystem();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }

        sub_retcode = connector_shutdown_dispatch_subsystem();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }

        sub_retcode = connector_shutdown_message_queue_subsystem();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }

        sub_retcode = connector_shutdown_trampoline_subsystem();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }

        sub_retcode = connector_cleanup_default_tcp_directory();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }

        /* Reset the memory allocators to the default */
        sub_retcode = connector_clear_allocators();
        if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            retcode = sub_retcode;
        }
    }

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                 SUBSTANCE_CONNECTOR_STATE_SHUTDOWN_STARTED,
                                 SUBSTANCE_CONNECTOR_STATE_SHUTDOWN,
                                 initialized);

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_set_allocators(substance_connector_memory_allocate_fp allocator,
                                           substance_connector_memory_free_fp deallocator)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    uint32_t initialized = 0u;

    /* Perform a guard on setting the memory allocators to ensure that
     * initialize and other API calls will not succeed while they are
     * being set. */
    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                 SUBSTANCE_CONNECTOR_STATE_SHUTDOWN,
                                 SUBSTANCE_CONNECTOR_STATE_ALLOCATOR_STARTED,
                                 initialized);

    /* Return error if the module is not in a clean shutdown state */
    if (initialized == SUBSTANCE_CONNECTOR_STATE_SHUTDOWN)
    {
        if (allocator != NULL && deallocator != NULL)
        {
            connector_set_allocator(allocator);
            connector_set_deallocator(deallocator);
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }

        CONNECTOR_ATOMIC_COMPARE_EXCHANGE(connector_module_state.state,
                                     SUBSTANCE_CONNECTOR_STATE_ALLOCATOR_STARTED,
                                     SUBSTANCE_CONNECTOR_STATE_SHUTDOWN,
                                     initialized);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_write_message(unsigned int context,
                                          const substance_connector_uuid_t *type,
                                          const char *message)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_INVALID;
    connector_message_t *connector_message = NULL;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        /* Build message structure */
        connector_message = connector_build_message(context, type, message);

        if (connector_message != NULL)
        {
            connector_message->context = context;
            connector_enqueue_outbound_message(connector_message);

            /* Signal to the write threads that there is a new message */
            connector_flag_write();

            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_add_trampoline(substance_connector_trampoline_fp
                                               trampoline)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_add_trampoline(trampoline);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_remove_trampoline(substance_connector_trampoline_fp
                                                  trampoline)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_remove_trampoline(trampoline);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_open_tcp(unsigned int port, unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED
        && context != NULL)
    {
        /* Create a new open context on a TCP socket */
        retcode = connector_context_open_tcp(port, context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_open_unix(const char *filepath,
                                      unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED
        && context != NULL)
    {
        retcode = connector_context_open_unix(filepath, context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_close_context(unsigned int context)
{
    return connector_context_close(context);
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_connect_tcp(unsigned int port,
                                        unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED
        && context != NULL)
    {
        retcode = connector_context_connect_tcp(port, context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_connect_unix(const char *filepath,
                                         unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED
        && context != NULL)
    {
        retcode = connector_context_connect_unix(filepath, context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_broadcast_tcp(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_broadcast_connect_tcp();
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_broadcast_unix(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_broadcast_connect_unix();
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_broadcast_default(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_broadcast_connect_default();
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_open_default_tcp(unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_open_default_tcp(context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_open_default_unix(unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_open_default_unix(context);
    }

    return retcode;
}

SUBSTANCE_CONNECTOR_EXPORT
unsigned int substance_connector_open_default(unsigned int *context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (connector_module_state.state == SUBSTANCE_CONNECTOR_STATE_INIT_FINISHED)
    {
        retcode = connector_open_default_connection(context);
    }

    return retcode;
}
