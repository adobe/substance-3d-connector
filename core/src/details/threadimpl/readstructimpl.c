/** @file readstructimpl.c
    @brief Contains the read thread structure implementation, separated
           from the read thread routines
    @author Galen Helfter - Adobe
    @date 20190726
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/threadimpl/readstructimpl.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/context_struct.h>

unsigned int connector_read_thread_remove_context(connector_read_thread_t *thread,
                                             unsigned int index)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (index < thread->assigned_contexts)
    {
        /* If there are remaining contexts, swap with the last one. Index
         * cannot be less than the assigned contexts if it is zero,
         * so this cannot underflow */
        thread->assigned_contexts -= 1u;

        if (thread->assigned_contexts > 0u)
        {
            thread->context_ids[index] =
                thread->context_ids[thread->assigned_contexts];

            thread->contexts[index] =
                thread->contexts[thread->assigned_contexts];
        }

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_read_thread_check_load(const connector_read_thread_t *thread)
{
    unsigned int result = SUBSTANCE_CONNECTOR_FALSE;

    if (thread->assigned_contexts < SUBSTANCE_CONNECTOR_READ_CONTEXTS)
    {
        result = SUBSTANCE_CONNECTOR_TRUE;
    }

    return result;
}

unsigned int connector_read_thread_try_acquire(connector_read_thread_t *thread)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    size_t index = 0u;
    unsigned int context = 0u;

    /* Attempt to grab another context */
    retcode = connector_available_queue_pop(&context);

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* Acquire the file descriptor from the context */
        index = thread->assigned_contexts;
        thread->contexts[index].fd = connector_context_get_fd(context);
        thread->contexts[index].events = POLLIN;
        thread->context_ids[index] = context;
        thread->assigned_contexts += 1u;

        /* Check whether the handshake has been sent and send it if not */
        retcode = connector_context_write_handshake(context);
    }

    return retcode;
}

unsigned int connector_read_thread_cleanup_connections(connector_read_thread_t *thread)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int context_state = 0u;
    unsigned int context = 0u;
    unsigned int item_removed = 0u;
    uint32_t i = 0u;

    /* Iterate over all of the thread's contexts */
    for (i = 0u; i < thread->assigned_contexts; ++i)
    {
        /* Backtrack one index if a context was removed */
        if (item_removed != 0u)
        {
            i -= 1u;
            item_removed = 0u;
        }

        /* Acquire the state of the current context */
        context = thread->context_ids[i];
        context_state = connector_context_state(context);

        if (context_state & SUBSTANCE_CONNECTOR_CONN_SHUTDOWN)
        {
            /* Take the context off of this internal list */
            retcode = connector_read_thread_remove_context(thread, (unsigned int) i);

            if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
            {
                item_removed = 1u;

                /* Perform a shutdown of the context */
                retcode = connector_context_shutdown_from_read_thread(context);
            }
        }
    }

    return retcode;
}

