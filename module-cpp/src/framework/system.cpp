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

//! @file system.cpp
//! @brief Core and system application type
//! @author Galen Helfter - Adobe
//! @date 20190616
//! @copyright Adobe. All rights reserved.

#include <substance/connector/framework/core.h>
#include <substance/connector/framework/system.h>
#include <substance/connector/framework/details/dynamicload.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
const substance_connector_uuid_t System::sConnectionEstablishedId = {
	// 02572bc5-2d84-450a-9e01-d22c66b1abb1
	0x02572bc5u,
	0x2d84450au,
	0x9e01d22cu,
	0x66b1abb1u};

const substance_connector_uuid_t System::sConnectionClosedId = {
	// 04705ddf-16d4-4489-af6c-6e3a93f1959d
	0x04705ddfu,
	0x16d44489u,
	0xaf6c6e3au,
	0x93f1959du};

	const substance_connector_uuid_t System::sConnectionUpdateContextId = {
	// 39b07a67-4996-4b62-a340-908a81f66562
	0x39b07a67u,
	0x49964b62u,
	0xa340908au,
	0x81f66562u};


System::System() : mRecvConnectionEstablished(nullptr), mRecvConnectionClosed(nullptr), mRecvConnectionContext(nullptr), mUserData(0x0u)
{
}

System::~System()
{
}

void System::preInit()
{
	// Initialize the callbacks
	mCallbacks.push_back(CallbackPair(sConnectionEstablishedId, {System::connectionEstablished, mRecvConnectionEstablished}));
	mCallbacks.push_back(CallbackPair(sConnectionClosedId, {mRecvConnectionClosed}));
	mCallbacks.push_back(CallbackPair(sConnectionUpdateContextId, {mRecvConnectionContext}));

	Application::preInit();
}

void System::postInit()
{
}

const std::vector<substance_connector_uuid_t> System::getFeatureIds() {
	return {
	  System::sConnectionEstablishedId,
	  System::sConnectionClosedId,
	};
}

void System::connectionEstablished(
	unsigned int context,
	const substance_connector_uuid_t* uuid,
	const char* message)
{
	auto context_message = getConnectionContext();
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &System::sConnectionUpdateContextId, context_message.c_str());
}

void System::postShutdown()
{
}
} // namespace Framework
} // namespace Connector
} // namespace Substance
