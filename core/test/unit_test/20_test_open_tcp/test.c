/** @file test.c
    @brief Testing that opening and closing a tcp socket works
    @author Galen Helfter - Adobe
    @date 20190806
    @copyright Adobe. All rights reserved.
*/

#include <substance/connector/common.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/details/connection_details.h>
#include <substance/connector/details/context_queue.h>
#include <substance/connector/details/context_struct.h>
#include <substance/connector/details/network/openconnectionimpl.h>
#include <substance/connector/details/system/connectiondirectory.h>

#include <common/test_common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEST_COUNT 1u

/* begin connector_test_ensure_unix_directory block */

static const char * _connector_test_open_close_tcp_errors[] =
{
    "Failed to initialize context queue",
    "Failed to initialize tcp directory",
    "Failed to create tcp file and open the context",
    "Failed to close the port file and the socket",
    "Failed to clean up tcp directory",
    "Failed to shutdown context queue"
};

static unsigned int _connector_test_open_close_tcp()
{
    unsigned int result = 0u;

    connector_context_t context;

    memset(&context, 0x00, sizeof(context));

    /* Must initialize the context queue to start up winsock2 on windows */
    if (connector_init_context_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 1u;
    }
    else if (connector_setup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 2u;
    }
    else if (connector_open_tcp_impl(&context) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 3u;
    }
    else if (connector_close_tcp(&context) != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 4u;
    }
    else if (connector_cleanup_default_tcp_directory() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 5u;
    }
    else if (connector_shutdown_context_subsystem() != SUBSTANCE_CONNECTOR_SUCCESS)
    {
        result = 6u;
    }

    return result;
}

/* end connector_test_ensure_unix_directory block */

/* List of tests for iteration */
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_open_close_tcp",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_test_open_close_tcp_errors,
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_test_open_close_tcp,
};

/* Test main function */
_CONNECTOR_TEST_MAIN
