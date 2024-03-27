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

//! @file callbacks.h
//! @brief Register callback types for messages
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#pragma once

#include <string>
#include <vector>
#include <substance/connector/framework/callbackstype.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{
//! @brief Bind the callback to the message type
//! @param messageType UUID for the type of message
//! @param callback Callback function called by connector
//! @return True on success, false if the message type already exists
bool bindCallback(const substance_connector_uuid_t& messageType, std::vector<MessageCallback> callbacks);

//! @brief Free the binding for the message type
//! @param messageType UUID for the type of message
//! @return True on success, false if no message of the type is bound
bool unbindCallback(const substance_connector_uuid_t& messageType);

//! @brief Search for the callback bound to the message type
//! @param messageType UUID for the type of message
//! @return Valid function on success, nullptr on failure
const std::vector<MessageCallback>& findCallbacks(const substance_connector_uuid_t& messageType);
} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
