/** @file test.c
    @brief Testing the available queue interface
    @author Galen Helfter - Adobe
    @date 20190626
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/available_queue.h>
#include <substance/connector/details/context_queue.h>

#include <common/test_common.h>

#define TEST_COUNT 2u

/* begin connector_test_available_queue_usage block */

static const char * _connector_test_available_queue_usage_errors[] =
{
    "Calling shutdown when uninitialized should result in failure",
    "Attempting to add a value when uninitialized should result in failure",
    "Attempting to remove a value when uninitialized should result in failure",
    "Failed pop from uninitialized queue should not modify value",
    "Failed to initialize the available queue",
    "Second initialization should result in an error",
    "Popping from an empty queue should result in an error",
    "Failed to add a value to the queue",
    "Failed to retrieve a value from the queue",
    "Retrieved result is not what was placed on the queue",
    "Failed to destroy the available queue"
};

static unsigned int _connector_test_available_queue_usage()
{
    unsigned int result = 0u;

    const unsigned int _original_value = 0x12875u;
    unsigned int _value = _original_value;
    unsigned int _target = 0x0u;

    if (connector_available_queue_shutdown() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_available_queue_push(12u) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_available_queue_pop(&_value) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (_value != _original_value)
    {
        result = 4u;
    }
    else if (connector_available_queue_init() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (connector_available_queue_init() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }
    else if (connector_available_queue_pop(&_target) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 7u;
    }
    else if (connector_available_queue_push(_original_value)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 8u;
    }
    else if (connector_available_queue_pop(&_target) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 9u;
    }
    else if (_target != _original_value)
    {
        result = 10u;
    }
    else if (connector_available_queue_shutdown() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 11u;
    }

    return result;
}

/* end connector_test_available_queue_usage block */

/* begin connector_test_available_queue_capacity  block */
static const char * _connector_test_available_queue_capacity_errors[] =
{
    "Failed to initialize the available queue",            /* 1 */
    "Failed to load the queue up to capacity",             /* 2 */
    "Failed to clear the queue",                           /* 3 */
    "Failed to load the queue to capacity after clearing", /* 4 */
    "Failed to clear the queue after second load",         /* 5 */
    "Failed to shut down the available queue"              /* 6 */
};

static unsigned int _test_capacity_load()
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    unsigned int i = 0u;
    unsigned int _successes = 0u;

    for (i = 0u; i < SUBSTANCE_CONNECTOR_CONTEXT_COUNT; ++i)
    {
        unsigned int _result = connector_available_queue_push(i);
        if (_result == SUBSTANCE_CONNECTOR_SUCCESS)
        {
            _successes += 1u;
        }
    }

    retcode = (_successes == SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
            ? SUBSTANCE_CONNECTOR_SUCCESS : SUBSTANCE_CONNECTOR_ERROR;

    return retcode;
}

static unsigned int _test_capacity_clear()
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;

    unsigned int i = 0u;
    unsigned int _successes = 0u;

    for (i = 0u; i < SUBSTANCE_CONNECTOR_CONTEXT_COUNT; ++i)
    {
        unsigned int _value = 0u;
        unsigned int _result = connector_available_queue_pop(&_value);
        if (_result == SUBSTANCE_CONNECTOR_SUCCESS && _value == i)
        {
            _successes += 1u;
        }
    }

    retcode = (_successes == SUBSTANCE_CONNECTOR_CONTEXT_COUNT)
            ? SUBSTANCE_CONNECTOR_SUCCESS : SUBSTANCE_CONNECTOR_ERROR;

    return retcode;
}

static unsigned int _connector_test_available_queue_capacity()
{
    unsigned int result = 0u;

    if (connector_available_queue_init() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (_test_capacity_load() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (_test_capacity_clear() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (_test_capacity_load() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (_test_capacity_clear() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (connector_available_queue_shutdown() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }

    return result;
}

/* end connector_test_available_queue_capacity  block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_available_queue_usage",
    "test_available_queue_capacity"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_available_queue_usage_errors,
    _connector_test_available_queue_capacity_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_available_queue_usage,
    _connector_test_available_queue_capacity
};

/* Test main function */
_CONNECTOR_TEST_MAIN
