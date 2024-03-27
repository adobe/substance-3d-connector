/** @file test.c
    @brief Test initializing, registering and calling trampoline functions
    @author Galen Helfter - Adobe
    @date 20190625
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/uuid_utils.h>

#include <common/test_common.h>

#include <string.h>

#define TEST_COUNT 2u

/* begin connector_test_trampoline_init block */

static const char * _connector_test_trampoline_init_errors[] =
{
    "Failed to initialize trampoline subsystem",
    "Failed to shutdown trampoline subsystem"
};

static unsigned int _connector_test_trampoline_init()
{
    unsigned int result = 0u;

    if (connector_init_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_shutdown_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }

    return result;
}

/* end connector_test_trampoline_init block */

/* begin connector_test_trampoline_call block */

static const char *_test_message = "Test Message";

static int _compare_message_result = -1;
static int _compare_uuid_result = -1;

static const substance_connector_uuid_t _test_uuid =
{
    /* 4a731ec7-8254-4ebe-a54f-fa1f3ae9d981 */
    {0x4a731ec7u, 0x82544ebeu, 0xa54ffa1fu, 0x3ae9d981u}
};

static void _test_trampoline(unsigned int context,
                             const substance_connector_uuid_t *type,
                             const char *message)
{
    SUBSTANCE_CONNECTOR_UNUSED(context);

    _compare_message_result = strcmp(_test_message, message);

    _compare_uuid_result = connector_compare_uuid(&_test_uuid, type);
}

static const char * _connector_test_trampoline_call_errors[] =
{
    "Failed to initialize trampoline subsystem",
    "Adding NULL trampoline did not result in an error",
    "Failed to add a valid trampoline",
    "Failed to call trampolines",
    "Results from trampoline are not what are expected.",
    "Failed to shutdown trampoline subsystem"
};

static unsigned int _connector_test_trampoline_call()
{
    unsigned int result = 0u;

    if (connector_init_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_add_trampoline(NULL) == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_add_trampoline(&_test_trampoline) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_notify_trampolines(0, &_test_uuid, _test_message)
             != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (_compare_message_result != 0u || _compare_uuid_result != 0u)
    {
        result = 5u;
    }
    else if (connector_shutdown_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }

    return result;
}

/* end connector_test_trampoline_call block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_trampoline_init",
    "test_trampoline_call"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_trampoline_init_errors,
    _connector_test_trampoline_call_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_trampoline_init,
    _connector_test_trampoline_call
};

/* Test main function */
_CONNECTOR_TEST_MAIN
