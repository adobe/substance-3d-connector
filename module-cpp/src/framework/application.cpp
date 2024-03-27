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

//! @file application.cpp
//! @brief Base application type for Connector
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/application.h>
#include <substance/connector/framework/details/callbacks.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
void Application::registerCallbacks() const
{
	for (const auto& callback : mCallbacks)
	{
		Details::bindCallback(callback.first, callback.second);
	}
}

void Application::unregisterCallbacks() const
{
	for (const auto& callback : mCallbacks)
	{
		Details::unbindCallback(callback.first);
	}
}

void Application::preInit()
{
	registerCallbacks();
}

void Application::preShutdown()
{
	unregisterCallbacks();

	mCallbacks.clear();
}
} // namespace Framework
} // namespace Connector
} // namespace Substance
