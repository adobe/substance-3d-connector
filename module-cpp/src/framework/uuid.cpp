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
