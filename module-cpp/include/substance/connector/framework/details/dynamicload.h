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

//! @file dynamicload.h
//! @brief Handle dynamic loading and calling of functions
//! @author Galen Helfter - Adobe
//! @date 20191029
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/connector.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{
#if defined(SUBSTANCE_CONNECTOR_FRAMEWORK_DYNAMIC_LOAD)
extern const substance_connector_function_table* connectorFunctionTable;
#define CONNECTOR_FRAMEWORK_CALL(fn) ::Substance::Connector::Framework::Details::connectorFunctionTable->fn
#else
#define CONNECTOR_FRAMEWORK_CALL(fn) substance_connector_ ## fn
#endif

//! @brief Acquire the dynamic loaded function table from the module
//! @param module Pointer to the dynamically loaded module
//! @return True on success, false on failure or if built without dynamic loading
bool initDynamicLoad(void* module);

//! @brief Clear the dynamically loaded function table
//! @return True on success, false on failure or if built without dynamic loading
bool shutdownDynamicLoad();
} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
