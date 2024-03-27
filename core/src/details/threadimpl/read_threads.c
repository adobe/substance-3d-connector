/** @file read_threads.c
    @brief Contains the functionality for the read threads
    @author Galen Helfter - Adobe
    @date 20190624
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/threadimpl/read_threads.h>

#include <string.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/disconnect_message.h>
#include <substance/connector/details/dispatch.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/thread.h>
#include <substance/connector/details/threadimpl/readstructimpl.h>
#include <substance/connector/details/threadimpl/readthreadimpl.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define SUBSTANCE_CONNECTOR_COMM_READ_DEFAULT NULL
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_COMM_READ_DEFAULT 0ul
#endif

static connector_thread_return_t read_thread_routine(void *data);

/* Shutdown flag to notify all read threads to shut down and exit. */
static unsigned int read_thread_shutdown_flag = 0u;

static connector_cond_t inbound_condition;
static connector_mutex_t inbound_lock;

static connector_cond_t read_main_condition;
static connector_mutex_t read_main_lock;

static void read_signal_main();

static connector_read_thread_t read_threads[SUBSTANCE_CONNECTOR_INBOUND_COUNT];

static void read_signal_main()
{
    connector_mutex_lock(&read_main_lock);
    connector_condition_broadcast(&read_main_condition);
    connector_mutex_unlock(&read_main_lock);
}

static connector_thread_return_t read_thread_routine(void *data)
{
    connector_thread_return_t result = SUBSTANCE_CONNECTOR_COMM_READ_DEFAULT;
    size_t i = 0u;
    unsigned int retcode = 0u;

    /* Expects that the data element is a pointer to the communication
     * read thread structure */
    connector_read_thread_t *thread = (connector_read_thread_t*) data;

    /* Main thread loop */
    while (read_thread_shutdown_flag == 0u && thread != NULL)
    {
        /* Await on the inbound condition, idling the thread until it
         * receives a context to handle. */
        retcode = connector_read_thread_await_condition(thread, &inbound_condition,
                                                   &inbound_lock,
                                                   &read_thread_shutdown_flag,
                                                   &read_signal_main);

        if (read_thread_shutdown_flag != 0)
        {
            /* Exit the loop and shut down */
            break;
        }

        /* Determine whether the load is okay to accept another context */
        if (connector_read_thread_check_load(thread) == SUBSTANCE_CONNECTOR_TRUE)
        {
            connector_read_thread_try_acquire(thread);
        }

        /* Poll all of the current contexts for incoming messages and
         * connections */
        if (thread->assigned_contexts > 0u)
        {
            retcode = SUBSTANCE_CONNECTOR_POLL_TIMEOUT;

            while (retcode == SUBSTANCE_CONNECTOR_POLL_TIMEOUT)
            {
                /* Clean up any shutdown connections at every iteration of
                 * the loop */
                connector_read_thread_cleanup_connections(thread);

                if (read_thread_shutdown_flag != 0u)
                {
                    /* If the thread has been flagged to shut down, exit before
                     * polling again */
                    goto thread_exit;
                }

                retcode = connector_poll_contexts(thread->contexts,
                                             thread->assigned_contexts);
            }
        }

        for (i = 0u; i < thread->assigned_contexts; ++i)
        {
            if (thread->contexts[i].revents & SUBSTANCE_CONNECTOR_POLLERR)
            {
                /* Some sort of error occurred - send disconnect message and close */
                connector_handle_error_disconnect(thread->context_ids[i]);

                connector_context_close(thread->context_ids[i]);
            }
            else if (thread->contexts[i].revents & SUBSTANCE_CONNECTOR_POLLHUP)
            {
                /* Force a message to call up in, regardless of if one was
                 * previously received. */
                connector_handle_error_disconnect(thread->context_ids[i]);

                /* Other side has elected to close the connection */
                retcode = connector_context_close(thread->context_ids[i]);
            }
            else if (thread->contexts[i].revents & SUBSTANCE_CONNECTOR_POLLIN)
            {
                /* There is input to handle on the given context */
                connector_read_thread_handle_context(thread->context_ids[i]);
            }
        }

        /* Perform another cleanup, which will handle any connections that
         * should be marked as shutdown */
        connector_read_thread_cleanup_connections(thread);
    }

    /* Exit the thread */
thread_exit:
    /* Close all connections and perform a cleanup on them */
    for (i = 0u; i < thread->assigned_contexts; ++i)
    {
        connector_context_close(thread->context_ids[i]);
    }

    connector_read_thread_cleanup_connections(thread);

    return result;
}

unsigned int connector_init_read_threads(void)
{
    unsigned int i = 0u;

    connector_condition_create(&inbound_condition);
    inbound_lock = connector_mutex_create();

    connector_condition_create(&read_main_condition);
    read_main_lock = connector_mutex_create();

    connector_mutex_lock(&read_main_lock);

    for (i = 0u; i < SUBSTANCE_CONNECTOR_INBOUND_COUNT; ++i)
    {
        read_threads[i].id = i;
        read_threads[i].assigned_contexts = 0u;

        read_threads[i].thread = connector_thread_create(&read_thread_routine,
                                                    &read_threads[i]);

        /* Block until the read threads have been initialized */
        connector_condition_wait(&read_main_condition, &read_main_lock);
    }

    connector_mutex_unlock(&read_main_lock);

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_shutdown_read_threads(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int i = 0u;
    unsigned int shutdown = 0u;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(read_thread_shutdown_flag, 0u, 1u, shutdown);

    if (shutdown == 0u)
    {
        connector_mutex_lock(&inbound_lock);
        connector_condition_broadcast(&inbound_condition);
        connector_mutex_unlock(&inbound_lock);

        for (i = 0u; i < SUBSTANCE_CONNECTOR_INBOUND_COUNT; ++i)
        {
            connector_thread_join(&read_threads[i].thread);

            connector_thread_destroy(&read_threads[i].thread);
        }

        CONNECTOR_ATOMIC_COMPARE_EXCHANGE(read_thread_shutdown_flag, 1u, 0u, shutdown);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;

        connector_mutex_destroy(&inbound_lock);
        connector_condition_destroy(&inbound_condition);

        connector_mutex_destroy(&read_main_lock);
        connector_condition_destroy(&read_main_condition);
    }

    return retcode;
}

unsigned int connector_flag_read_impl(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;

    /* Acquire mutex lock and signal the read threads to process if waiting
     * on the condition */
    connector_mutex_lock(&inbound_lock);
    connector_condition_broadcast(&inbound_condition);
    connector_mutex_unlock(&inbound_lock);

    return retcode;
}
