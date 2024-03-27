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
//! @brief QObject implementation of the system interface
//! @author Galen Helfter - Adobe
//! @date 20190815
//! @copyright Adobe. All rights reserved.

#include <atomic>

#include <substance/connector/qt/system.h>
#include <substance/connector/framework/system.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
static std::atomic<ConnectorSystem*> instance(nullptr);

static void recvConnectionEstablishedCallback(unsigned int context,
											  const substance_connector_uuid_t* uuid,
											  const char *message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvConnectionEstablished(context, QString(message));
	}
}

static void recvConnectionClosedCallback(unsigned int context,
										 const substance_connector_uuid_t* uuid,
										 const char *message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvConnectionClosed(context, QString(message));
	}
}

static void updateConnectionContextCallback(unsigned int context,
										 const substance_connector_uuid_t* uuid,
										 const char *message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->updateConnectionContext(context, QString(message));
	}
}

ConnectorSystem::ConnectorSystem(QObject* parent)
	: ConnectorApplication(parent)
{
}

ConnectorSystem::~ConnectorSystem()
{
}

void ConnectorSystem::initialize()
{
	ConnectorSystem* expected = nullptr;

	// Only a single instance should be valid
	if (instance.compare_exchange_strong(expected, this))
	{
		// Instantiate a new instance of the base framework system
		// application type
		mApplication = std::make_unique<Framework::System>();

		Framework::System* systemApplication = static_cast<Framework::System*>(mApplication.get());

		// Update the callback pointers
		systemApplication->mRecvConnectionEstablished = recvConnectionEstablishedCallback;
		systemApplication->mRecvConnectionClosed = recvConnectionClosedCallback;
		systemApplication->mRecvConnectionContext = updateConnectionContextCallback;
	}
}

void ConnectorSystem::shutdown()
{
	if (instance.load() == this)
	{
		mApplication = nullptr;

		// Only clear the atomic pointer after the shutdown has been completed
		ConnectorSystem* expected = this;
		instance.compare_exchange_strong(expected, nullptr);
	}
}
} // namespace Qt
} // namespace Connector
} // namespace Substance
