/** @file test.c
    @brief Testing functions for header network byte order conversions
    @author Galen Helfter - Allegorithmic
    @date 20190527
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/atomic.h>
#include <substance/connector/details/callbacks.h>
#include <substance/connector/details/dispatch.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_queue.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>

#if defined(SUBSTANCE_CONNECTOR_WIN32)
#elif defined(SUBSTANCE_CONNECTOR_POSIX)
#include <unistd.h>
#endif

#define TEST_COUNT 1u

/* begin connector_test_dispatch_callback block */

static const char * _connector_test_dispatch_callback_errors[] =
{
    "Failed to initialize trampoline system",
    "Failed to add trampoline",
    "Failed to initialize message queue system",
    "Failed to initialize dispatch system",
    "Failed to build message",
    "Failed to flag the dispatch threads",
    "Failed to shut down dispatch system",
    "Failed to shut down message queue system",
    "Failed to shut down trampoline system",
    "Failed to have callback fired"
};

static unsigned int _test_callback_value = 0u;

static void _test_callback(unsigned int context,
                           const substance_connector_uuid_t *uuid,
                           const char *message)
{
    SUBSTANCE_CONNECTOR_UNUSED(context);
    SUBSTANCE_CONNECTOR_UNUSED(uuid);
    SUBSTANCE_CONNECTOR_UNUSED(message);

    CONNECTOR_ATOMIC_SET_1(_test_callback_value);
}

static const substance_connector_uuid_t _test_uuid =
{
    /* 3ce27ff1-5e79-45a4-9ffa-47516143b154 */
    {0x3ce27ff1u, 0x5e7945a4u, 0x9ffa4751u, 0x6143b154u}
};

static const char *_message_payload = "Test message";

static unsigned int _connector_test_dispatch_callback()
{
    connector_message_t *message = NULL;

    if (connector_init_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 1u;
    }

    if (connector_add_trampoline(_test_callback) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 2u;
    }

    if (connector_init_message_queue_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 3u;
    }

    if (connector_init_dispatch_subsystem(SUBSTANCE_CONNECTOR_TRUE)
        != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 4u;
    }

    /* Test firing the callback, append a message */
    message = connector_build_message(0u, &_test_uuid, _message_payload);
    if (message == NULL)
    {
        return 5u;
    }

    connector_enqueue_inbound_message(message);

    message = connector_build_message(0u, &_test_uuid, _message_payload);
    connector_enqueue_inbound_message(message);

    /* Signal a dispatch thread to process. This will block on a condition
     * variable and await the completion of the dispatch thread */
    connector_await_dispatch();

    if (connector_shutdown_dispatch_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 7u;
    }

    if (connector_shutdown_message_queue_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 8u;
    }

    if (connector_shutdown_trampoline_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        return 9u;
    }

    if (_test_callback_value != 1u)
    {
        return 10u;
    }

    return 0u;
}

/* end connector_test_create_message block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_dispatch_callback"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_dispatch_callback_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_dispatch_callback
};

/* Test main function */
_CONNECTOR_TEST_MAIN
