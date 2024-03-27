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

//! @file callbacks.cpp
//! @brief Register callback types for messages
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/details/callbacks.h>
#include <substance/connector/framework/uuid.h>

#include <map>
#include <vector>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{
static std::map<substance_connector_uuid_t, std::vector<MessageCallback>> callbackMap;

bool bindCallback(const substance_connector_uuid_t& messageType, std::vector<MessageCallback> callbacks)
{
	bool result = false;

	if (callbackMap.find(messageType) == callbackMap.end())
	{
		callbackMap[messageType] = callbacks;
		result = true;
	}

	return result;
}

bool unbindCallback(const substance_connector_uuid_t& messageType)
{
	bool result = false;

	auto found = callbackMap.find(messageType);
	if (found != callbackMap.end())
	{
		callbackMap.erase(found);
		result = true;
	}

	return result;
}

//NOTE:: This should be improved. Added to avoid warning of returning local variable
static std::vector<MessageCallback> _empty_callbacks;
const std::vector<MessageCallback>& findCallbacks(const substance_connector_uuid_t& messageType)
{
	auto found = callbackMap.find(messageType);
	if (found != callbackMap.end())
	{
		return found->second;
	}

	return _empty_callbacks;
}
} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
