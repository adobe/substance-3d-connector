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

//! @file uuid.h
//! @brief Higher level utilities for using uuids with connector
//! @author Galen Helfter - Adobe
//! @date 20190525
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/types.h>

// Operators must be in the global namespace, as the C struct is also
// in the global namespace.

//! @brief Less-than comparison operator for a connector uuid type
//! @param x Left-hand element to compare
//! @param y Right-handle element being compared to
//! @return True if x < y, false otherwise
bool operator <(const substance_connector_uuid_t& x, const substance_connector_uuid_t& y);

//! @brief Equality comparison operator for a connector uuid type
//! @param x Left-hand element to compare
//! @param y Right-handle element being compared to
//! @return True if x == y, false otherwise
bool operator ==(const substance_connector_uuid_t& x, const substance_connector_uuid_t& y);
