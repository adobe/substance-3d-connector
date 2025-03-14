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
//! @brief Test startup and shutdown of a basic system
//! @author Galen Helfter - Adobe
//! @date 20190526
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/details/callbacks.h>
#include <substance/connector/framework/application.h>
#include <substance/connector/framework/core.h>

#include <common/test_common.h>
#include <cstdlib>

#define TEST_COUNT 1u

// begin connector_fw_test_init_shutdown block

void _test_callback(unsigned int context, const substance_connector_uuid_t *uuid, const char *messagew)
{
    // Do nothing
}

class _TestApplication : public Substance::Connector::Framework::Application
{
public:
    // Set constructor and destructor to default
    _TestApplication()
        : mPreInitCalled(false)
        , mPostInitCalled(false)
        , mPreShutdownCalled(false)
        , mPostShutdownCalled(false)
    {
    }
    ~_TestApplication() = default;

    void preInit() override
    {
        mPreInitCalled = true;
    }

    void postInit() override
    {
        mPostInitCalled = true;
    }

    void preShutdown() override
    {
        mPreShutdownCalled = true;
    }

    void postShutdown() override
    {
        mPostShutdownCalled = true;
    }

	const std::vector<substance_connector_uuid_t> getFeatureIds() override
    {
        return {};
    }

    // Flags for determining whether the state callbacks are fired
    bool mPreInitCalled;
    bool mPostInitCalled;
    bool mPreShutdownCalled;
    bool mPostShutdownCalled;
};

static const char * _connector_fw_test_init_shutdown_errors[] =
{
    "Failed to register application",
    "Failed to initialize",
    "Failed to call pre init",
    "Failed to call post init",
    "Failed to shut down",
    "Failed to call pre shutdown",
    "Failed to call post shutdown"
};

unsigned int _connector_fw_test_init_shutdown()
{
    unsigned int result = 0u;

    _TestApplication application;

    if (!Substance::Connector::Framework::registerApplication(&application))
    {
        result = 1u;
    }
    else if (!Substance::Connector::Framework::init("Test Application"))
    {
        result = 2u;
    }
    else if (!application.mPreInitCalled)
    {
        result = 3u;
    }
    else if (!application.mPostInitCalled)
    {
        result = 4u;
    }
    else if (!Substance::Connector::Framework::shutdown())
    {
        result = 5u;
    }
    else if (!application.mPreShutdownCalled)
    {
        result = 5u;
    }
    else if (!application.mPostShutdownCalled)
    {
        result = 6u;
    }

    return result;
}

// end connector_fw_test_init_shutdown block

// List of tests for iteration
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_fw_init_shutdown",
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_fw_test_init_shutdown_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_fw_test_init_shutdown
};

// Test main function
_CONNECTOR_TEST_MAIN
