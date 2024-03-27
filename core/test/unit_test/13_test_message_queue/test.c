/** @file test.c
    @brief Testing the message queue interface
    @author Galen Helfter - Adobe
    @date 20190626
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/types.h>
#include <substance/connector/details/memory.h>
#include <substance/connector/details/message_queue.h>

#include <common/test_common.h>

#define TEST_COUNT 1u

/* begin connector_test_message_queue_usage block */

static const char * const _test_payload = "Test Payload";

static const substance_connector_uuid_t _test_uuid =
{
    /* 659adc0c-5083-4db7-8fea-ae3d3024c3d3 */
    {0x659adc0cu, 0x50834db7u, 0x8feaae3du, 0x3024c3d3u}
};

static unsigned int _test_retrieve_message()
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_message_t *message = NULL;

    message = connector_acquire_inbound_message();

    if (message != NULL)
    {
        connector_clear_message(message);
        connector_free(message);

        retcode = SUBSTANCE_CONNECTOR_SUCCESS;
    }

    return retcode;
}

static unsigned int _test_add_retrieve_message()
{
    unsigned int retcode = SUBSTANCE_CONNECTOR_ERROR;
    connector_message_t *message = NULL;
    connector_message_t *retrieved = NULL;

    message = connector_build_message(0u, &_test_uuid, _test_payload);

    if (message != NULL)
    {
        connector_enqueue_inbound_message(message);

        retrieved = connector_acquire_inbound_message();

        if (retrieved == message)
        {
            retcode = SUBSTANCE_CONNECTOR_SUCCESS;
        }

        connector_clear_message(retrieved);
        connector_free(retrieved);
    }

    return retcode;
}

static const char * _connector_test_message_queue_usage_errors[] =
{
    "Calling shutdown when uninitialized should result in failure",
    "Failed to initialize",
    "Second initialization should result in failure",
    "Retrieving a message without emplacing anything should return nothing",
    "Adding and retrieving a message should acquire the same object",
    "Failed shutdown after initialization"
};

static unsigned int _connector_test_message_queue_usage()
{
    unsigned int result = 0u;

    if (connector_shutdown_message_queue_subsystem() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_init_message_queue_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_init_message_queue_subsystem() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (_test_retrieve_message() == SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (_test_add_retrieve_message() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (connector_shutdown_message_queue_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }

    return result;
}

/* end connector_test_message_queue_usage block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_message_queue_usage",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_message_queue_usage_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_message_queue_usage
};

/* Test main function */
_CONNECTOR_TEST_MAIN
