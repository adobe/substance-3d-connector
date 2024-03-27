/** @file readthreadimpl.c
    @brief Contains implementation functions used by the read threads
    @author Galen Helfter - Adobe
    @date 20190801
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/threadimpl/readthreadimpl.h>

#include <string.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/dispatch.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/thread.h>
#include <substance/connector/details/threadimpl/read_threads.h>
#include <substance/connector/details/threadimpl/readstructimpl.h>

unsigned int connector_read_thread_handle_open(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int new_context = 0u;

    /* Open context - accept the connection and assign it to the
     * appropriate thread */
    retcode = connector_context_accept(context, &new_context);

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* On success, append the new context to the available connection
         * queue */
        connector_flag_read_impl();
    }

    return retcode;
}

unsigned int connector_read_thread_handle_connected(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_message_t *message = NULL;

    const size_t allocation_size = sizeof(connector_message_t) +
                                   sizeof(connector_message_header_t);

    message = connector_allocate(allocation_size);

    memset(message, 0x00, allocation_size);
    message->context = context;
    message->header = (connector_message_header_t*) ((uint8_t*) message +
                                                sizeof(connector_message_t));

    retcode = connector_context_read(context, message);

    if (retcode == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        /* Enqueue an inbound message and fire dispatch threads */
        connector_enqueue_inbound_message(message);

        connector_flag_dispatch();
    }
    else
    {
        /* Clean up memory on a failed read */
        connector_free(message);
    }

    return retcode;
}

unsigned int connector_read_thread_await_condition(struct _connector_read_thread *thread,
                                              connector_cond_t *condition,
                                              connector_mutex_t *lock,
                                              unsigned int *shutdown_flag,
                                              connector_read_signal_main_fp notify)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    connector_mutex_lock(lock);

    if (notify != NULL)
    {
        notify();
    }

    while (thread->assigned_contexts == 0u && *shutdown_flag == 0)
    {
        /* Await the condition variable to be signaled */
        connector_condition_wait(condition, lock);

        if (*shutdown_flag != 0u)
        {
            /* Exit the function call if the shutdown code has been set */
            break;
        }

        /* Attempt to acquire a new connection */
        connector_read_thread_try_acquire(thread);
    }

    /* Successfully acquired a connection */
    if (thread->assigned_contexts > 0u)
    {
        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    /* Release the mutex on completion */
    connector_mutex_unlock(lock);

    return retcode;
}

unsigned int connector_read_thread_handle_context(unsigned int context)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int context_state = 0u;

    context_state = connector_context_state(context);

    if (context_state == SUBSTANCE_CONNECTOR_CONN_OPEN)
    {
        retcode = connector_read_thread_handle_open(context);
    }
    else if (context_state == SUBSTANCE_CONNECTOR_CONN_CONNECTED)
    {
        retcode = connector_read_thread_handle_connected(context);
    }
    else
    {
        /* There is a problem if a context is marked as closed and still
         * has data to read. */
        retcode = SUBSTANCE_CONNECTOR_INVALID;
    }

    return retcode;
}

unsigned int connector_read_thread_handle_poll(connector_read_thread_t *thread)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int i = 0u;

    for (i = 0u; i < thread->assigned_contexts; ++i)
    {
        if (thread->contexts[i].revents & SUBSTANCE_CONNECTOR_POLLERR)
        {
            retcode = connector_context_close(thread->context_ids[i]);
        }
        else if (thread->contexts[i].revents & SUBSTANCE_CONNECTOR_POLLHUP)
        {
            retcode = connector_context_close(thread->context_ids[i]);
        }
    }

    return retcode;
}
