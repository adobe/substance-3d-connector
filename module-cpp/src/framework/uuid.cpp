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

//! @file uuid.cpp
//! @brief Higher level utilities for using uuids
//! @author Galen Helfter - Adobe
//! @date 20190525
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/uuid.h>

#include <iomanip>
#include <sstream>
#include <tuple>

using uuid_tuple = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>;

bool operator<(const substance_connector_uuid_t& x, const substance_connector_uuid_t& y)
{
	return uuid_tuple(x.elements[0], x.elements[1], x.elements[2], x.elements[3]) <
		   uuid_tuple(y.elements[0], y.elements[1], y.elements[2], y.elements[3]);
}

bool operator==(const substance_connector_uuid_t& x, const substance_connector_uuid_t& y)
{
	return uuid_tuple(x.elements[0], x.elements[1], x.elements[2], x.elements[3]) ==
		   uuid_tuple(y.elements[0], y.elements[1], y.elements[2], y.elements[3]);
}

namespace Substance
{
namespace Connector
{
namespace Framework
{
std::string uuidToString(const substance_connector_uuid_t& uuid)
{
	std::stringstream ss;
	auto& ints = uuid.elements;

	// Format the UUID in the canonical 8-4-4-4-12 format
	ss << std::hex << std::setw(8) << std::setfill('0') << ints[0] << '-' << std::setw(4) << std::setfill('0')
	   << (ints[1] >> 16)                                                // high part of ints[1]
	   << '-' << std::setw(4) << std::setfill('0') << (ints[1] & 0xFFFF) // low part of ints[1]
	   << '-' << std::setw(4) << std::setfill('0') << (ints[2] >> 16)    // high part of ints[3]
	   << '-' << std::setw(4) << std::setfill('0') << (ints[2] & 0xFFFF) // Last 12 chars
	   << std::setw(8) << std::setfill('0') << ints[3];

	return ss.str();
}
substance_connector_uuid_t uuidFromString(const std::string& uuid_string)
{
	substance_connector_uuid_t uuid = {0x0u, 0x0u, 0x0u, 0x0u};
	uint8_t currentElementIndex = 0;
	std::string currentElementString = "";
	for (const auto& c : uuid_string)
	{
		if (c == '-')
			continue;
		currentElementString += c;
		if (currentElementString.length() == 8)
		{
			uint32_t u;
			std::istringstream iss(currentElementString);
			iss >> std::setbase(16) >> u;
			uuid.elements[currentElementIndex] = u;
			currentElementIndex++;
			currentElementString = "";
			if (currentElementIndex >= 4)
			{
				break;
			}
		}
	}
	return uuid;
}
} // namespace Framework
} // namespace Connector
} // namespace Substance