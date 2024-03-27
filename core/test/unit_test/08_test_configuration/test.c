/** @file test.c
    @brief Testing configuration functions
    @author Galen Helfter - Adobe
    @date 20190625
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/configuration.h>

#include <common/test_common.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEST_COUNT 1u

/* begin connector_test_application_name block */

static const char * _connector_test_application_name_errors[] =
{
    "Failed to receive error on NULL input",
    "Failed to assign a valid application name",
    "Retrieved application name is not correct"
};

static const char * const application_name = "Test Application";

static unsigned int _connector_test_application_name()
{
    unsigned int result = 0u;

    if (connector_set_application_name(NULL) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_set_application_name(application_name)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (strcmp(connector_get_application_name(), application_name) != 0)
    {
        result = 3u;
    }

    return result;
}

/* end connector_test_application_name block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_application_name"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_application_name_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_application_name
};

/* Test main function */
_CONNECTOR_TEST_MAIN
