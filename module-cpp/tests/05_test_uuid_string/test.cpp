/*
Copyright 2025 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

//! @file test.cpp
//! @brief Testing functions for the UUID from and to string conversion
//! @author Camille Masson - Adobe
//! @date 20250122
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/uuid.h>

#include <common/test_common.h>
#include <cstdlib>

#define TEST_COUNT 2u

// _connector_fw_test_uuid_to_string block
static const char* _connector_fw_test_uuid_to_string_errors[] =
{
    "Failed UUID to String conversion",
    "Failed zero-filled UUID to String conversion",
};

static unsigned int _connector_fw_test_uuid_to_string()
{
    unsigned int result = 0u;
    substance_connector_uuid_t uuid;
    std::string uuidstring;

    uuid = { 0x4d7b79b5u, 0xa6bc4a0fu, 0xbe84462eu, 0x148cf29cu };
    uuidstring = Substance::Connector::Framework::uuidToString(uuid);
    if (uuidstring != "4d7b79b5-a6bc-4a0f-be84-462e148cf29c")
        result = 1u;

    uuid = { 0x0u, 0x0u, 0x0u, 0x0u };
    uuidstring = Substance::Connector::Framework::uuidToString(uuid);
    if (uuidstring != "00000000-0000-0000-0000-000000000000")
        result = 2u;

    uuid = { 0x0000000fu, 0xf0000000u, 0xf0001000u, 0x00000001u };
    uuidstring = Substance::Connector::Framework::uuidToString(uuid);
    if (uuidstring != "0000000f-f000-0000-f000-100000000001")
        result = 2u;

    return result;
}
// end _connector_fw_test_uuid_to_string block

// _connector_fw_test_string_to_uuid block
static const char* _connector_fw_test_string_to_uuid_errors[] =
{
    "Failed String to UUID conversion",
    "Failed zero String to UUID conversion",
    "Failed String without dashes to UUID conversion",
    "Failed Python formatted string UUID conversion",
    "Failed too large string UUID conversion",
    "Failed too small string UUID conversion",
    "Failed invalid string UUID conversion",
};

static unsigned int _connector_fw_test_string_to_uuid()
{
    unsigned int result = 0u;
    substance_connector_uuid_t uuid;

    uuid = Substance::Connector::Framework::uuidFromString("4d7b79b5-a6bc4a0f-be84462e-148cf29c");
    if (!(uuid == substance_connector_uuid_t({ 0x4d7b79b5u, 0xa6bc4a0fu, 0xbe84462eu, 0x148cf29cu })))
        result = 1u;

    uuid = Substance::Connector::Framework::uuidFromString("");
    if (!(uuid == substance_connector_uuid_t({ 0x0u, 0x0u, 0x0u, 0x0u })))
        result = 2u;

    uuid = Substance::Connector::Framework::uuidFromString("12345678123456781234567812345678");
    if (!(uuid == substance_connector_uuid_t({ 0x12345678u, 0x12345678u, 0x12345678u, 0x12345678u })))
        result = 3u;

    uuid = Substance::Connector::Framework::uuidFromString("91e3dfbc-80b8-4b1a-92d5-63ec09ac641a");
    if (!(uuid == substance_connector_uuid_t({ 0x91e3dfbcu, 0x80b84b1au, 0x92d563ecu, 0x09ac641au })))
        result = 4u;

    uuid = Substance::Connector::Framework::uuidFromString("91e3dfbc-80b8-4b1a-92d5-63ec09ac641a-auiernstauie");
    if (!(uuid == substance_connector_uuid_t({ 0x91e3dfbcu, 0x80b84b1au, 0x92d563ecu, 0x09ac641au })))
        result = 5u;

    uuid = Substance::Connector::Framework::uuidFromString("91e3dfbc-80b8-4b1a-92d5-63");
    if (!(uuid == substance_connector_uuid_t({ 0x91e3dfbcu, 0x80b84b1au, 0x0u, 0x0u })))
        result = 6u;

    uuid = Substance::Connector::Framework::uuidFromString("91jjdf←c-80b8-4b1a-92d5-63ec09ac641a");
    if (!(uuid == substance_connector_uuid_t({ 0x91, 0x0, 0x1a92d563u, 0xec09ac64u })))
        result = 7u;

    return result;
}
// end _connector_fw_test_string_to_uuid block


// List of tests for iteration
static const char* _connector_test_names[TEST_COUNT] =
{
    "test_uuid_to_string",
    "test_string_to_uuid"
};

static const char** _connector_test_errors[TEST_COUNT] =
{
    _connector_fw_test_uuid_to_string_errors,
    _connector_fw_test_string_to_uuid_errors
};

static const _connector_test_fp _connector_test_functions[TEST_COUNT] =
{
    _connector_fw_test_uuid_to_string,
    _connector_fw_test_string_to_uuid,
};

// Test main function
_CONNECTOR_TEST_MAIN
