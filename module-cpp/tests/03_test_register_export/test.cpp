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
//! @brief Test the creation of a basic export application
//! @author Galen Helfter - Adobe
//! @date 20190526
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/details/callbacks.h>
#include <substance/connector/framework/features/sendsbsar.h>

#include <common/test_common.h>
#include <cstdlib>

#define TEST_COUNT 1u

// begin connector_fw_test_register_export block

static Substance::Connector::Framework::SbsarApplication sbsarApplication;

void _connector_test_import_callback(unsigned int context,
                                const substance_connector_uuid_t *messageType,
                                const char *message)
{
}

void _connector_test_update_callback(unsigned int context,
                                const substance_connector_uuid_t *messageType,
                                const char *message)
{
}

static const char * _connector_fw_test_register_export_errors[] =
{
    "Failed to find load callback after preinitialization",
    "Failed to find update callback after preinitialization",
    "Failed to clear load callback",
    "Failed to clear update callback"
};

unsigned int _connector_fw_test_register_export()
{
    unsigned int result = 0u;

    // Register the callbacks
    sbsarApplication.mRecvLoadSbsar = _connector_test_import_callback;
    sbsarApplication.mRecvUpdateSbsar = _connector_test_update_callback;

    // This will register all of the callbacks with the core
    sbsarApplication.preInit();

    using Substance::Connector::Framework::Details::findCallbacks;
    using Substance::Connector::Framework::Details::unbindCallback;
    using Substance::Connector::Framework::SbsarApplication;

    // Assert that we can locate the callbacks using their uuids
    if (findCallbacks(SbsarApplication::sLoadSbsarId)[0] != _connector_test_import_callback)
    {
        result = 1u;
    }
    else if (findCallbacks(SbsarApplication::sUpdateSbsarId)[0] != _connector_test_update_callback)
    {
        result = 2u;
    }
    else if (!unbindCallback(SbsarApplication::sLoadSbsarId))
    {
        result = 3u;
    }
    else if (!unbindCallback(SbsarApplication::sUpdateSbsarId))
    {
        result = 4u;
    }

    return result;
}

// end connector_fw_test_register_export block

// List of tests for iteration
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_fw_register_export",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_fw_test_register_export_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_fw_test_register_export
};

// Test main function
_CONNECTOR_TEST_MAIN
