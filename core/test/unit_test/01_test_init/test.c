/** @file test.c
    @brief Testing functions for initialization
    @author Galen Helfter - Allegorithmic
    @date 20190506
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/connector.h>
#include <substance/connector/errorcodes.h>

#include <common/test_common.h>

#include <stdint.h>
#include <stdlib.h>

#define TEST_COUNT 1u

/* connector_test_init block */
static const char * _connector_test_init_errors[] =
{
    "Failed initialization.",
    "Failed shutdown."
};

static unsigned int _connector_test_init()
{
    unsigned int result = 0u;

    if (substance_connector_init("test") != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (substance_connector_shutdown() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }

    return result;
}

/* end connector_test_init block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_init"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_init_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_init
};

/* Test main function */
_CONNECTOR_TEST_MAIN
