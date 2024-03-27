/** @file test.c
    @brief Testing different pieces of the read threads, ensuring functionality
    @author Galen Helfter - Adobe
    @date 20190805
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/thread.h>
#include <substance/connector/details/threadimpl/readstructimpl.h>
#include <substance/connector/details/threadimpl/readthreadimpl.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 1u

/* begin connector_test_await_connection block */

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#define _CONNECTOR_TEST_THREAD_RESULT NULL
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#define _CONNECTOR_TEST_THREAD_RESULT 0ul
#endif

static unsigned int _connector_test_shutdown_flag = 0u;
static connector_cond_t _connector_test_condition;
static connector_mutex_t _connector_test_lock;

static connector_cond_t _connector_test_main_condition;
static connector_mutex_t _connector_test_main_lock;

typedef void (*_connector_test_thread_signal_main_fp)();

typedef struct _connector_test_thread
{
    connector_read_thread_t read_thread;
    unsigned int retcode;
    _connector_test_thread_signal_main_fp signal_main;
} connector_test_thread_t;

static void _connector_test_signal_main()
{
    connector_mutex_lock(&_connector_test_main_lock);
    connector_condition_signal(&_connector_test_main_condition);
    connector_mutex_unlock(&_connector_test_main_lock);
}

static connector_thread_return_t _connector_test_routine(void *data)
{
    connector_thread_return_t result = _CONNECTOR_TEST_THREAD_RESULT;
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    connector_test_thread_t *thread = data;

    retcode = connector_read_thread_await_condition(&thread->read_thread,
                                               &_connector_test_condition,
                                               &_connector_test_lock,
                                               &_connector_test_shutdown_flag,
                                               thread->signal_main);

    /* Pass the return value back out to the main thread */
    thread->retcode = retcode;

    if (thread->signal_main != NULL)
    {
        thread->signal_main();
    }

    return result;
}

static const char * _connector_test_await_condition_errors[] =
{
    "failed to initialize the available queue",
    "Failed to successfully flag the read thread and acquire a context",
    "Failed to shut down the available queue",
};

static unsigned int _connector_test_await_condition()
{
    unsigned int result = 0u;

    connector_test_thread_t _thread;

    memset(&_thread, 0x00, sizeof(_thread));

    if (connector_available_queue_init() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else
    {
        connector_condition_create(&_connector_test_condition);
        connector_condition_create(&_connector_test_main_condition);
        _connector_test_lock = connector_mutex_create();
        _connector_test_main_lock = connector_mutex_create();

        connector_mutex_lock(&_connector_test_main_lock);

        _thread.retcode = SUBSTANCE_CONNECTOR_ERROR;
        _thread.signal_main = _connector_test_signal_main;
        _thread.read_thread.thread = connector_thread_create(&_connector_test_routine,
                                                        &_thread);

        connector_condition_wait(&_connector_test_main_condition, &_connector_test_main_lock);

        /* Append a connection to the thread and flag the condition */
        connector_mutex_lock(&_connector_test_lock);

        connector_available_queue_push(1u);

        connector_condition_signal(&_connector_test_condition);

        connector_mutex_unlock(&_connector_test_lock);

        /* Await the signal from the worker thread */
        connector_condition_wait(&_connector_test_main_condition, &_connector_test_main_lock);
        connector_mutex_unlock(&_connector_test_main_lock);

        connector_thread_join(&_thread.read_thread.thread);

        /* Test that the thread has returned correctly */
        if (_thread.retcode != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = 2u;
        }
        else if (connector_available_queue_shutdown() != SUBSTANCE_CONNECTOR_SUCCESS)
        {
            result = 3u;
        }
    }

    return result;
}

/* end connector_test_await_connection block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_await_condition",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_await_condition_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_await_condition,
};

/* Test main function */
_CONNECTOR_TEST_MAIN
