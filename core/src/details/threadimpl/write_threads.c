/** @file write_threads.c
    @brief Contains the functionality for the write threads
    @author Galen Helfter - Adobe
    @date 20190624
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/details/threadimpl/write_threads.h>

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/thread.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define SUBSTANCE_CONNECTOR_COMM_WRITE_DEFAULT NULL
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_COMM_WRITE_DEFAULT 0ul
#endif

#ifndef SUBSTANCE_CONNECTOR_OUTBOUND_COUNT
#define SUBSTANCE_CONNECTOR_OUTBOUND_COUNT 2
#endif

typedef struct _connector_write_thread
{
    connector_thread_t thread;
    unsigned int id;
} connector_write_thread_t;

static connector_thread_return_t write_thread_routine(void *data);

/* Shutdown flag to notify all write threads to shut down and exit */
static unsigned int write_thread_shutdown_flag = 0u;

static connector_cond_t outbound_condition;
static connector_mutex_t outbound_lock;

static connector_write_thread_t write_threads[SUBSTANCE_CONNECTOR_OUTBOUND_COUNT];

static connector_thread_return_t write_thread_routine(void *data)
{
    connector_thread_return_t result = SUBSTANCE_CONNECTOR_COMM_WRITE_DEFAULT;
    connector_message_t *message = NULL;
    unsigned int sub_retcode = 0u;

    /* Expects that the data element is a pointer to the communication
     * thread structure */
    connector_write_thread_t *thread = (connector_write_thread_t*) data;

    /* Main thread loop */
    while (write_thread_shutdown_flag  == 0u && thread != NULL)
    {
        connector_mutex_lock(&outbound_lock);

        /* Check for anything on the outbound queue */
        while (message == NULL)
        {
            /* Sleep until the outbound message is fired again */
            connector_condition_wait(&outbound_condition, &outbound_lock);

            if (write_thread_shutdown_flag != 0u)
            {
                /* If the shutdown state is set, then exit the thread */
                connector_mutex_unlock(&outbound_lock);
                goto thread_exit;
            }

            message = connector_acquire_outbound_message();
        }

        connector_mutex_unlock(&outbound_lock);

        while (message != NULL)
        {
            sub_retcode = connector_context_write(message->context, message);

            if (sub_retcode != SUBSTANCE_CONNECTOR_SUCCESS)
            {
                /* Handle failed connection */
            }

            /* Delete the message */
            connector_clear_message(message);
            connector_free(message);

            message = connector_acquire_outbound_message();
        }
    }

thread_exit:
    /* Clean up message data */
    if (message != NULL)
    {
        connector_free(message->message);
        connector_free(message);
        message = NULL;
    }

    return result;
}

unsigned int connector_init_write_threads(void)
{
    unsigned int i = 0u;

    connector_condition_create(&outbound_condition);
    outbound_lock = connector_mutex_create();

    for (i = 0u; i < SUBSTANCE_CONNECTOR_OUTBOUND_COUNT; ++i)
    {
        write_threads[i].id = i;
        write_threads[i].thread = connector_thread_create(&write_thread_routine,
                                                     &write_threads[i]);
    }

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_shutdown_write_threads(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    unsigned int i = 0u;
    unsigned int shutdown = 0u;

    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(write_thread_shutdown_flag, 0u, 1u, shutdown);

    if (shutdown == 0u)
    {
        connector_mutex_lock(&outbound_lock);
        connector_condition_broadcast(&outbound_condition);
        connector_mutex_unlock(&outbound_lock);

        /* Join and destroy all writing threads */
        for (i = 0u; i < SUBSTANCE_CONNECTOR_OUTBOUND_COUNT; ++i)
        {
            connector_thread_join(&write_threads[i].thread);

            connector_thread_destroy(&write_threads[i].thread);
        }

        /* Clear shutdown code */
        CONNECTOR_ATOMIC_COMPARE_EXCHANGE(write_thread_shutdown_flag, 1u, 0u, shutdown);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;

        connector_mutex_destroy(&outbound_lock);
        connector_condition_destroy(&outbound_condition);
    }

    return retcode;
}

unsigned int connector_flag_write_impl(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_SUCCESS;

    /* Acquire mutex lock and signal the write threads to process if waiting
     * on the condition */
    connector_mutex_lock(&outbound_lock);
    connector_condition_signal(&outbound_condition);
    connector_mutex_unlock(&outbound_lock);

    return retcode;
}
