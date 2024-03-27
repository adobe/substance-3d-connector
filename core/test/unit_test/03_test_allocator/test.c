/** @file test.c
    @brief Testing functions for the allocators interface
    @author Galen Helfter - Allegorithmic
    @date 20190510
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/connector.h>
#include <substance/connector/errorcodes.h>

#include <common/test_common.h>

#include <stdlib.h>

#define TEST_COUNT 1u

/* connector_test_set_alloc block */
static unsigned int _test_alloc_flag = 0u;
static unsigned int _test_dealloc_flag = 0u;

static void* _test_alloc(size_t size)
{
    _test_alloc_flag += 1u;
    return malloc(size);
}

static void _test_dealloc(void *ptr)
{
    _test_dealloc_flag += 1u;
    free(ptr);
}

static const char * _connector_test_set_alloc_errors[] =
{
    "Failed to set the allocators"
};

static unsigned int _connector_test_set_alloc()
{
    unsigned int result = 0u;

    if (substance_connector_set_allocators(_test_alloc, _test_dealloc) !=
        SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }

    return result;
}

/* end connector_test_set_alloc block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_set_alloc"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_set_alloc_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_set_alloc
};

/* Test main function */
_CONNECTOR_TEST_MAIN
