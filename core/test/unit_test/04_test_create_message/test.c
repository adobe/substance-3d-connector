/** @file test.c
    @brief Testing functions for creating a message
    @author Galen Helfter - Allegorithmic
    @date 20190510
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/message.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/uuid_utils.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>

#define TEST_COUNT 1u

/* connector_test_create_message block */
static const char * _connector_test_create_message_errors[] =
{
    "Failed to allocate message structure",
    "Message type does not match what is provided",
    "Payload does not match what was provided",
    "Context does not match"
};

static unsigned int _connector_test_create_message()
{
    unsigned int result = 0u;
    const unsigned int context = 2u;

    /* dc52784e-62d1-4731-a8cf-a4cffbb718b8 */
    const substance_connector_uuid_t uid =
    {
        {0xdc52784eu, 0x62d14731u, 0xa8cfa4cfu, 0xfbb718b8u}
    };

    const char *const payload = "test_message";

    connector_message_t *message = connector_build_message(context, &uid, payload);

    if (message == NULL)
    {
        result = 1u;
    }
    else if (connector_compare_uuid(&uid, &message->header->message_id) != 0)
    {
        result = 2u;
    }
    else if (message->context != context)
    {
        result = 4u;
    }

    return result;
}

/* end connector_test_create_message block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_create_message"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_create_message_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_create_message
};

/* Test main function */
_CONNECTOR_TEST_MAIN
