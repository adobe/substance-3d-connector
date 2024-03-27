/** @file test.c
    @brief Testing functions for the UUID struct
    @author Galen Helfter - Allegorithmic
    @date 20190422
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/types.h>
#include <substance/connector/details/uuid_utils.h>

#include <common/test_common.h>

#include <stdlib.h>

#define TEST_COUNT 1u

/* connector_test_compare block */
static const char * _connector_test_compare_errors[] =
{
    "Failed equivalence test.",
    "Failed less than test.",
    "Failed greater than test."
};

static unsigned int _connector_test_compare()
{
    unsigned int result = 0;

    /* 94ff9731-08f4-4942-8973-d1e0ba164294 */
    substance_connector_uuid_t id0 =
    {
        {0x94ff9731u, 0x08f44942u, 0x8973d1e0u, 0xba164294u}
    };

    /* b80fe03c-7f9a-42d8-9d46-7f8f43550a1b */
    substance_connector_uuid_t id1 =
    {
        {0xb80fe03cu, 0x7f9a42d8u, 0x9d467f8fu, 0x43550a1bu}
    };

    if (connector_compare_uuid(&id0, &id0) != 0)
    {
        result = 1u;
    }
    else if (connector_compare_uuid(&id0, &id1) != -1)
    {
        result = 2u;
    }
    else if (connector_compare_uuid(&id1, &id0) != 1)
    {
        result = 3u;
    }

    return result;
}

/* end connector_test_compare block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_compare",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_compare_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_compare,
};

/* Test main function */
_CONNECTOR_TEST_MAIN
