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
//! @brief Testing functions for the UUID comparison operators
//! @author Galen Helfter - Adobe
//! @date 20190525
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/uuid.h>

#include <common/test_common.h>
#include <cstdlib>

#define TEST_COUNT 2u

// connector_fw_test_less_than block

static const char * _connector_fw_test_less_than_errors[] =
{
    "Failed less than comparison when less than other value",
    "Failed less than comparison when equal to other value",
    "Failed less than comparison when greater than other value"
};

static unsigned int _connector_fw_test_less_than()
{
    unsigned int result = 0u;

    // 4d7b79b5-a6bc-4a0f-be84-462e148cf29c
    substance_connector_uuid_t uuid0 = {0x4d7b79b5u, 0xa6bc4a0fu, 0xbe84462eu, 0x148cf29cu};

    // edb81e21-11fc-434d-aa4c-316accbd1451
    substance_connector_uuid_t uuid1 = {0xedb81e21u, 0x11fc434du, 0xaa4c316au, 0xccbd1451u};

    if (!(uuid0 < uuid1))
    {
        result = 1u;
    }
    else if (uuid0 < uuid0)
    {
        result = 2u;
    }
    else if (uuid1 < uuid0)
    {
        result = 3u;
    }

    return result;
}

// end connector_fw_test_less_than block

// connector_fw_test_equals block

static const char * _connector_fw_test_equals_errors[] =
{
    "Failed equality check when equal",
    "Failed equality check when not equal",
    "Failed equality check when close but not equal"
};

static unsigned int _connector_fw_test_equals()
{
    unsigned int result = 0u;

    // 7eb8029f-103d-407c-9d8d-e02de1d55ca5
    substance_connector_uuid_t uuid0 = {0x7eb8029fu, 0x103d407cu, 0x9d8de02du, 0xe1d55ca5u};

    // ed9916a6-6cba-43cf-8bfc-85ad6f5669c7
    substance_connector_uuid_t uuid1 = {0xed9916a6u, 0x6cba43cfu, 0x8bfc85adu, 0x6f5669c7u};
    substance_connector_uuid_t uuid2 = {0xed9916a6u, 0x6cba43cfu, 0x8bfc85adu, 0xe1d55ca5u};


    if (!(uuid0 == uuid0))
    {
        result = 1u;
    }
    else if (uuid0 == uuid1)
    {
        result = 2u;
    }
    else if (uuid1 == uuid2)
    {
        result = 3u;
    }

    return result;
}

// end connector_fw_test_equals block

// List of tests for iteration
static const char * _connector_test_names[TEST_COUNT] =
{
    "test_fw_uuid_less_than",
    "test_fw_uuid_equals"
};

static const char ** _connector_test_errors[TEST_COUNT] =
{
    _connector_fw_test_less_than_errors,
    _connector_fw_test_equals_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_fw_test_less_than,
    _connector_fw_test_equals
};

// Test main function
_CONNECTOR_TEST_MAIN
