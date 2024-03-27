/** @file test.c
    @brief Testing the uint queue used for contexts, etc.
    @author Galen Helfter - Adobe
    @date 20190626
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/uint_queue.h>

#include <common/test_common.h>

#include <stdint.h>

#define TEST_COUNT 1u

/* begin connector_test_uint_queue_usage block */

static const char * _connector_test_uint_queue_usage_errors[] =
{
    "Failed to create a uint queue",
    "Calling pop on an empty queue erroneously returned successful",
    "Calling push on a NULL queue should return an error",
    "Calling destroy on a NULL queue should return an error",
    "Pushing a value onto a created queue should be successful",
    "Failed to retrieve a value after successfully adding one",
    "Retrieved value is not what was placed on the queue",
    "Failed to destroy a valid queue"
};

static unsigned int _connector_test_uint_queue_usage()
{
    unsigned int result = 0u;
    const uint32_t _queue_size = 64u;

    unsigned int _queue_pop_value = 0u;
    const unsigned int _queue_test_value = 0x12u;

    connector_uint_queue_t *_queue = NULL;

    if ((_queue = connector_uint_queue_create(_queue_size)) == NULL)
    {
        result = 1u;
    }
    else if (connector_uint_queue_pop(_queue, &_queue_pop_value)
             == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_uint_queue_push(NULL, 0x00) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_uint_queue_destroy(NULL) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (connector_uint_queue_push(_queue, _queue_test_value)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (connector_uint_queue_pop(_queue, &_queue_pop_value)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }
    else if (_queue_pop_value != _queue_test_value)
    {
        result = 7u;
    }
    else if (connector_uint_queue_destroy(_queue) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 8u;
    }

    return result;
}

/* end connector_test_uint_queue_usage block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_uint_queue_usage"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_uint_queue_usage_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_uint_queue_usage
};

/* Test main function */
_CONNECTOR_TEST_MAIN
