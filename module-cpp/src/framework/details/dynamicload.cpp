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

//! @file dynamicload.cpp
//! @brief Handle dynamic loading and calling of functions
//! @author Galen Helfter - Adobe
//! @date 20191029
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/details/dynamicload.h>
#include <substance/connector/common.h>

#if defined(SUBSTANCE_CONNECTOR_POSIX)
#include <dlfcn.h>
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#include <windows.h>
#endif

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{
#if defined(SUBSTANCE_CONNECTOR_FRAMEWORK_DYNAMIC_LOAD)
const substance_connector_function_table* connectorFunctionTable;
#endif

static void* connectorDynamicLoadAcquireSymbol(void* module, const char* name)
{
	void* symbol = nullptr;

#if defined(SUBSTANCE_CONNECTOR_POSIX)
	symbol = dlsym(module, name);
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
	symbol = static_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
#endif

	return symbol;
}

bool initDynamicLoad(void* module)
{
	bool result = false;

#if defined(SUBSTANCE_CONNECTOR_FRAMEWORK_DYNAMIC_LOAD)
	if (module != nullptr)
	{
		// Acquire the symbol from the module
		void* symbol = connectorDynamicLoadAcquireSymbol(module, "substance_connector_module_table");

		if (symbol != nullptr)
		{
			connectorFunctionTable = static_cast<substance_connector_function_table*>(symbol);
			result = true;
		}
	}
#endif

	return result;
}

bool shutdownDynamicLoad()
{
	bool result = false;

#if defined(SUBSTANCE_CONNECTOR_FRAMEWORK_DYNAMIC_LOAD)
	connectorFunctionTable = nullptr;
	result = true;
#endif

	return result;
}
} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
