/*
Copyright 2024 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

//! @file sendsbsar.cpp
//! @brief QObject implementation of the send sbsar to interface
//! @author Daniel Stover - Adobe
//! @date 20242108
//! @copyright Adobe. All rights reserved.

#include <atomic>

#include <substance/connector/qt/features/sendsbsar.h>
#include <substance/connector/framework/features/sendsbsar.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
static std::atomic<SbsarApplication*> instance(nullptr);

static void recvLoadSbsarCallback(unsigned int context,
								  const substance_connector_uuid_t* uuid,
								  const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvLoadSbsar(context, QString(message));
	}
}

static void recvUpdateSbsarCallback(unsigned int context,
									const substance_connector_uuid_t* uuid,
									const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvUpdateSbsar(context, QString(message));
	}
}

SbsarApplication::SbsarApplication(QObject* parent) : ConnectorApplication(parent)
{
}

SbsarApplication::~SbsarApplication()
{
}

void SbsarApplication::initialize()
{
	SbsarApplication* expected = nullptr;

	// Only a single instance should be valid at a given time
	if (instance.compare_exchange_strong(expected, this))
	{
		// Instantiate a new instance of the base framework sbsarApplication
		// application type
		mApplication = std::make_unique<Framework::SbsarApplication>();

		Framework::SbsarApplication* sbsarApplication =
			static_cast<Framework::SbsarApplication*>(mApplication.get());

		// Update the callback pointers
		sbsarApplication->mRecvLoadSbsar = recvLoadSbsarCallback;
		sbsarApplication->mRecvUpdateSbsar = recvUpdateSbsarCallback;
	}
}

void SbsarApplication::shutdown()
{
	if (instance.load() == this)
	{
		mApplication = nullptr;

		// Only clear the atomic pointer after the shutdown has been completed
		SbsarApplication* expected = this;
		instance.compare_exchange_strong(expected, nullptr);
	}
}

void SbsarApplication::sendLoadSbsar(unsigned int context,
									 Substance::Connector::Framework::Schemas::send_to_schema& schema)
{
	Framework::SbsarApplication* sbsarApplication =
		static_cast<Framework::SbsarApplication*>(mApplication.get());

	if (sbsarApplication != nullptr)
	{
		sbsarApplication->sendLoadSbsar(context, schema);
	}
}

void SbsarApplication::sendLoadSbsar(unsigned int context, const QString& message)
{
	Framework::SbsarApplication* sbsarApplication =
		static_cast<Framework::SbsarApplication*>(mApplication.get());

	if (sbsarApplication != nullptr)
	{
		sbsarApplication->sendLoadSbsar(context, message.toUtf8().constData());
	}
}

} // namespace Qt
} // namespace Connector
} // namespace Substance
