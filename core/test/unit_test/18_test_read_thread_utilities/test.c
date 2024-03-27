/** @file test.c
    @brief Testing different pieces of the read threads, ensuring functionality
    @author Galen Helfter - Adobe
    @date 20190726
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/configuration.h>
#include <substance/connector/details/threadimpl/readstructimpl.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 3u

/* begin connector_test_remove_context block */

static const char * _connector_test_remove_context_errors[] =
{
    "Failed to remove the first context",
    "Failed to swap the last context with the given index",
    "Assigned contexts should have been decremented."
};

static unsigned int _connector_test_remove_context()
{
    unsigned int result = 0u;

    connector_read_thread_t _test_thread;

    memset(&_test_thread, 0x00, sizeof(_test_thread));

    /* Perform initialization */
    unsigned int _context_count = 5u;
    _test_thread.assigned_contexts = _context_count;

    for (unsigned int i = 0u; i < _context_count; ++i)
    {
        _test_thread.context_ids[i] = i;
    }

    if (connector_read_thread_remove_context(&_test_thread, 0u)
        != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (_test_thread.context_ids[0] != (_context_count - 1u))
    {
        result = 2u;
    }
    else if (_test_thread.assigned_contexts != (_context_count - 1u))
    {
        result = 3u;
    }

    return result;
}

/* end connector_test_remove_context block */

/* begin connector_test_try_acquire block */

static const char * _connector_test_try_acquire_errors[] =
{
    "Failed to initialize available queue",
    "Failed to add test context to the queue",
    "Failed to set the application name",
    "Could not perform a try and acquire with the default write operation",
    "Failed to shut down the available queue",
};

static unsigned int _connector_test_try_acquire()
{
    unsigned int result = 0u;

    connector_read_thread_t _test_thread;

    memset(&_test_thread, 0x00, sizeof(_test_thread));

    const unsigned int _test_context = 12u;
    unsigned int retcode = 0u;

    if (connector_available_queue_init() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_available_queue_push(_test_context) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_set_application_name("Test Application")
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    /* Try-acquire will now return success if the context is not set as
     * connected. */
    else if ((retcode = connector_read_thread_try_acquire(&_test_thread))
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (connector_available_queue_shutdown() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }

    connector_clear_application_name();

    return result;
}

/* end connector_test_try_acquire block */

/* begin connector_test_cleanup_connections block */

static const char * _connector_test_cleanup_connections_errors[] =
{
    "Failed to remove the first context",
    "Failed to swap the last context with the given index",
    "Assigned contexts should have been decremented."
};

static unsigned int _connector_test_cleanup_connections()
{
    unsigned int result = 0u;

    connector_read_thread_t _test_thread;

    memset(&_test_thread, 0x00, sizeof(_test_thread));

    return result;
}

/* end connector_test_cleanup_connections block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_remove_context",
    "test_try_acquire",
    "test_cleanup_connections",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_remove_context_errors,
    _connector_test_try_acquire_errors,
    _connector_test_cleanup_connections_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_remove_context,
    _connector_test_try_acquire,
    _connector_test_cleanup_connections,
};

/* Test main function */
_CONNECTOR_TEST_MAIN
