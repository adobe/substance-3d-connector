/** @file dispatch.c
    @brief Contains the functionality for dispatch thread processing
    @author Galen Helfter - Allegorithmic
    @date 20181215
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/details/dispatch.h>

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/internal_messages.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/message_queue.h>
#include <substance/connector/details/state.h>
#include <substance/connector/details/thread.h>

#include <stdlib.h>

/* Set default return values based on specific platform */
#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define SUBSTANCE_CONNECTOR_DISPATCH_DEFAULT NULL
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define SUBSTANCE_CONNECTOR_DISPATCH_DEFAULT 0ul
#endif

/* Amount of dispatch threads */
#ifndef SUBSTANCE_CONNECTOR_DISPATCH_COUNT
#define SUBSTANCE_CONNECTOR_DISPATCH_COUNT 2u
#endif

typedef struct _connector_dispatch_thread
{
    connector_thread_t thread;
    unsigned int initialized;
    unsigned int id;
} connector_dispatch_thread_t;

/* Dispatch thread condition variable */
static connector_cond_t dispatch_condition;

/* Condition variable for signaling the main thread */
static connector_cond_t dispatch_main_condition;

/* Array of dispatch threads */
static connector_dispatch_thread_t dispatch_threads[SUBSTANCE_CONNECTOR_DISPATCH_COUNT];

/* Dispatch thread shutdown information */
/* Shutdown flag to tell all dispatch threads to shut down. All operations
 * performed should be atomic */
static unsigned int dispatch_shutdown_code = 0u;

/* Flag to set telling the dispatch system that it should provide a signal when
 * successfully acquiring a message. */
static unsigned int dispatch_signal_main = SUBSTANCE_CONNECTOR_FALSE;

static connector_mutex_t dispatch_lock;

static connector_mutex_t dispatch_main_lock;

/* Local function declarations */
static connector_thread_return_t dispatch_routine(void *data);   
static unsigned int terminate_dispatch();

static void dispatch_signal_main_condition(void)
{
    if (dispatch_signal_main == SUBSTANCE_CONNECTOR_TRUE)
    {
        connector_mutex_lock(&dispatch_main_lock);
        connector_condition_broadcast(&dispatch_main_condition);
        connector_mutex_unlock(&dispatch_main_lock);
    }
}

static connector_thread_return_t dispatch_routine(void *data)
{
    connector_thread_return_t result = SUBSTANCE_CONNECTOR_DISPATCH_DEFAULT;
    connector_message_t *message = NULL;

    /* Expects that the data element is a pointer to the dispatch thread
     * structure */
    connector_dispatch_thread_t *thread = (connector_dispatch_thread_t*) data;

    while (dispatch_shutdown_code == 0u && thread != NULL)
    {
        /* Check if there is anything on the inbound queue. Perform this
         * check here to ensure that there is only one mutex acquired
         * at any time. */
        message = connector_acquire_inbound_message();

        connector_mutex_lock(&dispatch_lock);

        while (message == NULL && dispatch_shutdown_code == 0u)
        {
            thread->initialized = SUBSTANCE_CONNECTOR_TRUE;

            /* Sleep until the dispatch condition is fired again */
            connector_condition_wait(&dispatch_condition, &dispatch_lock);

            /* Unlock the mutex, as it has been reclaimed after awakening
             * from the dispatch condition. */
            connector_mutex_unlock(&dispatch_lock);

            if (dispatch_shutdown_code != 0u)
            {
                /* Shut down */
                goto thread_exit;
            }

            /* Acquire message off the message queue */
            message = connector_acquire_inbound_message();

            if (message == NULL)
            {
                /* Signal main that the thread is processing, if configured. */
                dispatch_signal_main_condition();
            }

            /* Reacquire the lock as the prerequisite for the
             * condition wait call. */
            connector_mutex_lock(&dispatch_lock);
        }

        connector_mutex_unlock(&dispatch_lock);

        /* Fire callbacks with the message -- Add check for shutdown code */
        while (message != NULL && dispatch_shutdown_code == 0u)
        {
            /* Check whether the message is internal, then call internal if
             * so */
            if (CONNECTOR_IDENTIFY_INTERNAL(message->header->description))
            {
                connector_call_internal_message(message->context,
                                           &message->header->message_id,
                                           message->message);
            }
            else
            {
                /* Process the inbound message by notifying all of the 
                 * trampolines of the incoming message */
                connector_notify_trampolines(message->context,
                                        &message->header->message_id,
                                        message->message);
            }

            /* Delete the message */
            connector_clear_message(message);
            connector_free(message);

            /* Try to acquire a new message to process */
            message = connector_acquire_inbound_message();
        }

        dispatch_signal_main_condition();
    }

    /* Exit the thread */
thread_exit:
    /* Clean up message data */
    if (message != NULL)
    {
        connector_clear_message(message);
        connector_free(message);
        message = NULL;
    }

    return result;
}

static unsigned int terminate_dispatch(void)
{
    unsigned int i = 0u;
    unsigned int expected = 1u;

    /* Fire dispatch signal to force sleeping threads to awaken */

    /* Set shutdown code - all threads should read this and shut down */
    connector_mutex_lock(&dispatch_lock);
    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(dispatch_shutdown_code, 0u, 1u, expected);
    connector_condition_broadcast(&dispatch_condition);
    connector_mutex_unlock(&dispatch_lock);

    /* Go through each thread and join with them */
    for (i = 0u; i < SUBSTANCE_CONNECTOR_DISPATCH_COUNT; ++i)
    {
        /* Join the thread, then shut down any locks associated with it */
        connector_thread_join(&dispatch_threads[i].thread);

        connector_thread_destroy(&dispatch_threads[i].thread);
    }

    connector_mutex_lock(&dispatch_lock);
    CONNECTOR_ATOMIC_COMPARE_EXCHANGE(dispatch_shutdown_code, 1u, 0u, expected);
    connector_mutex_unlock(&dispatch_lock);

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_flag_dispatch(void)
{
    /* Signal the dispatch thread condition variables */
    connector_mutex_lock(&dispatch_lock);
    connector_condition_signal(&dispatch_condition);
    connector_mutex_unlock(&dispatch_lock);

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_await_dispatch(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    if (dispatch_signal_main == SUBSTANCE_CONNECTOR_TRUE)
    {
        connector_mutex_lock(&dispatch_main_lock);

        connector_flag_dispatch();

        connector_condition_wait(&dispatch_main_condition, &dispatch_main_lock);

        connector_mutex_unlock(&dispatch_main_lock);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

unsigned int connector_init_dispatch_subsystem(unsigned int signal_main)
{
    unsigned int i = 0u;
    unsigned int initialized = SUBSTANCE_CONNECTOR_FALSE;

    /* Initialize condition variable for signaling worker threads */
    connector_condition_create(&dispatch_condition);
    dispatch_lock = connector_mutex_create();

    if (signal_main == SUBSTANCE_CONNECTOR_TRUE)
    {
        dispatch_signal_main = SUBSTANCE_CONNECTOR_TRUE;
        connector_condition_create(&dispatch_main_condition);
        dispatch_main_lock = connector_mutex_create();
    }

    /* Initialize the threads */
    for (i = 0u; i < SUBSTANCE_CONNECTOR_DISPATCH_COUNT; ++i)
    {
        dispatch_threads[i].id = i;
        dispatch_threads[i].initialized = SUBSTANCE_CONNECTOR_FALSE;
        dispatch_threads[i].thread = connector_thread_create(dispatch_routine,
                                                        &dispatch_threads[i]);
    }

    /* Spin until all of the threads are initialized */
    while (initialized != SUBSTANCE_CONNECTOR_TRUE)
    {
        connector_mutex_lock(&dispatch_lock);
        initialized = SUBSTANCE_CONNECTOR_TRUE;
        for (i = 0u; i < SUBSTANCE_CONNECTOR_DISPATCH_COUNT; ++i)
        {
            if (dispatch_threads[i].initialized != SUBSTANCE_CONNECTOR_TRUE)
            {
                initialized = SUBSTANCE_CONNECTOR_FALSE;
                break;
            }
        }
        connector_mutex_unlock(&dispatch_lock);
    }

    return SUBSTANCE_CONNECTOR_SUCCESS;
}

unsigned int connector_shutdown_dispatch_subsystem(void)
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    /* Shutdown all threads */
    retcode = terminate_dispatch();

    /* Delete condition variable */
    connector_mutex_destroy(&dispatch_lock);
    connector_condition_destroy(&dispatch_condition);

    return retcode;
}
