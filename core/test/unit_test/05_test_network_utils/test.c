/** @file test.c
    @brief Testing functions for header network byte order conversions
    @author Galen Helfter - Allegorithmic
    @date 20190510
    @copyright Allegorithmic. All rights reserved.
*/

#include <substance/connector/errorcodes.h>
#include <substance/connector/details/connection_utils.h>
#include <substance/connector/details/message_header.h>
#include <substance/connector/details/uuid_utils.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 1u

/* begin connector_test_create_message block */
static unsigned int compare_headers(const struct _connector_message_header_r1 *h0,
                                    const struct _connector_message_header_r1 *h1)
{
    unsigned int result = 0u;

    if (h0->description != h1->description)
    {
        result = 1u;
    }
    else if (h0->message_length != h1->message_length)
    {
        result = 2u;
    }
    else if (connector_compare_uuid(&h0->message_id, &h1->message_id) != 0)
    {
        result = 3u;
    }

    return result;
}

static const char * _connector_test_byte_conversions_errors[] =
{
    "Pieces of the header are not in network byte order",
    "Reverting back to host order did not get the original value"
};

static unsigned int _connector_test_byte_conversions()
{
    unsigned int result = 0u;

    struct _connector_message_header_r1 header_host;
    struct _connector_message_header_r1 header_network;
    struct _connector_message_header_r1 header_host_2;

    /* b058d3a8-4c15-4981-9ec4-5343a32483e4 */
    const substance_connector_uuid_t uid =
    {
        {0xb058d3a8u, 0x4c154981u, 0x9ec45343u, 0xa32483e4u}
    };

    header_host.description = CONNECTOR_MESSAGE_IDENTIFIER;
    header_host.message_length = 19273u;
    header_host.message_id = uid;

    connector_htonheader(&header_network, &header_host);

    connector_ntohheader(&header_host_2, &header_network);

    if (compare_headers(&header_host_2, &header_host) != 0)
    {
        result = 1u;
    }

    return result;
}

/* end connector_test_create_message block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_byte_conversions",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_byte_conversions_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_byte_conversions,
};

/* Test main function */
_CONNECTOR_TEST_MAIN
