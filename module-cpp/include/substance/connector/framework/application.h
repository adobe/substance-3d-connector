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

//! @file application.h
//! @brief Base application type for Connector
//! @author Galen Helfter - Adobe
//! @date 20190410
//! @copyright Adobe. All rights reserved.

#pragma once

#include <vector>
#include <utility>
#include <substance/connector/framework/callbackstype.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class Application
{
public:
	//! @brief Virtual destructor
	virtual ~Application() = default;

	//! @brief Virtual method to run preinitialize steps
	virtual void preInit();

	//! @brief Virtual method to run postinitialize steps
	virtual void postInit() = 0;

	//! @brief Virtual method to run preshutdown steps
	virtual void preShutdown();

	//! @brief Virtual method to run postshutdown steps
	virtual void postShutdown() = 0;

	//! @brief Virtual method to return supported feature ids
	virtual const std::vector<substance_connector_uuid_t> getFeatureIds() = 0;

protected:
	using CallbackPair = std::pair<substance_connector_uuid_t, std::vector<MessageCallback>>;
	std::vector<CallbackPair> mCallbacks;

	//! @brief Register all of the callbacks in mCallbacks
	void registerCallbacks() const;

	//! @brief Unregister all of the callbacks in mCallbacks
	void unregisterCallbacks() const;
};
} // namespace Framework
} // namespace Connector
} // namespace Substance
