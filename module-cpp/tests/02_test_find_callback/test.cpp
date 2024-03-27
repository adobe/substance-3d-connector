/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

//! @file test.cpp
//! @brief Test the binding and searching of callbacks to uuid messages
//! @author Galen Helfter - Adobe
//! @date 20190525
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/details/callbacks.h>

#include <common/test_common.h>
#include <cstdlib>
#include <vector>

#define TEST_COUNT 2u

void _test_callback(unsigned int context, const substance_connector_uuid_t *uuid, const char *message)
{
    // Do nothing
}

// begin connector_fw_test_add_callback block

static const char * _connector_fw_test_add_callback_errors[] =
{
    "Failed to add the first callback",
    "Adding an already used uuid did not fail",
    "Removing an existing callback failed"
};

unsigned int _connector_fw_test_add_callback()
{
    unsigned int result = 0u;

    // e890d6e1-b903-421b-b453-5127e6efa174
    substance_connector_uuid_t uuid = {0xe890d6e1u, 0xb903421bu, 0xb4535127u, 0xe6efa174u};

    if (!Substance::Connector::Framework::Details::bindCallback(uuid, _test_callback))
    {
        result = 1u;
    }
    else if (Substance::Connector::Framework::Details::bindCallback(uuid, _test_callback))
    {
        // Second attempt to register the callback should fail
        result = 2u;
    }
    else if (!Substance::Connector::Framework::Details::unbindCallback(uuid))
    {
        // Failed to remove callback after it has been registered
        result = 3u;
    }

    return result;
}

// end connector_fw_test_add_callback block

// begin connector_fw_test_find_callback block

static const char * _connector_fw_test_find_callback_errors[] =
{
    "Unregistered ID failed to return null",
    "Registering callback and uuid failed",
    "Could not recover correct callback from map",
    "Failed to remove registered callback"
};

unsigned int _connector_fw_test_find_callback()
{
    unsigned int result = 0u;

    // babff00d-26f2-4591-ba58-1d9c23ffba38
    substance_connector_uuid_t uuid0 = {0xbabff00du, 0x26f24591u, 0xba581d9cu, 0x23ffba38u};

    if (Substance::Connector::Framework::Details::findCallbacks(uuid0).size() != 0)
    {
        // Invalid callback received for unregistered ID
        result = 1u;
    }
    else if (!Substance::Connector::Framework::Details::bindCallback(uuid0, {_test_callback}))
    {
        // Failed to register valid callback
        result = 2u;
    }
    else if (Substance::Connector::Framework::Details::findCallbacks(uuid0).size() == 0)
    {
        // Returned incorrect result
        result = 3u;
    }
    else if (!Substance::Connector::Framework::Details::unbindCallback(uuid0))
    {
        result = 4u;
    }

    return result;
}

// end connector_fw_test_find_callback block

// List of tests for iteration
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_fw_add_callback",
    "test_fw_find_callback"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_fw_test_add_callback_errors,
    _connector_fw_test_find_callback_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_fw_test_add_callback,
    _connector_fw_test_find_callback
};

// Test main function
_CONNECTOR_TEST_MAIN
