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

//! @file export.cpp
//! @brief QObject implementation of the export interface
//! @author Galen Helfter - Adobe
//! @date 20190607
//! @copyright Adobe. All rights reserved.

#include <atomic>

#include <substance/connector/qt/export.h>
#include <substance/connector/framework/export.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
static std::atomic<ConnectorExport*> instance(nullptr);

static void recvLoadSbsarCallback(unsigned int context,
								  const substance_connector_uuid_t* uuid,
								  const char *message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvLoadSbsar(context, QString(message));
	}
}

static void recvLoadAssetCallback(unsigned int context,
								  const substance_connector_uuid_t* uuid,
								  const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvLoadAsset(context, QString(message));
	}
}

static void recvUpdateSbsarCallback(unsigned int context,
									const substance_connector_uuid_t* uuid,
									const char *message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvUpdateSbsar(context, QString(message));
	}
}

ConnectorExport::ConnectorExport(QObject* parent)
	: ConnectorApplication(parent)
{
}

ConnectorExport::~ConnectorExport()
{
}

void ConnectorExport::initialize()
{
	ConnectorExport* expected = nullptr;

	// Only a single instance should be valid at a given time
	if (instance.compare_exchange_strong(expected, this))
	{
		// Instantiate a new instance of the base framework export
		// application type
		mApplication = std::make_unique<Framework::Export>();

		Framework::Export* exportApplication = static_cast<Framework::Export*>(mApplication.get());

		// Update the callback pointers
		exportApplication->mRecvLoadSbsar = recvLoadSbsarCallback;
		exportApplication->mRecvLoadAsset = recvLoadAssetCallback;
		exportApplication->mRecvUpdateSbsar = recvUpdateSbsarCallback;
	}
}

void ConnectorExport::shutdown()
{
	if (instance.load() == this)
	{
		mApplication = nullptr;

		// Only clear the atomic pointer after the shutdown has been completed
		ConnectorExport* expected = this;
		instance.compare_exchange_strong(expected, nullptr);
	}
}

void ConnectorExport::sendLoadSbsar(unsigned int context, const QString& message)
{
	Framework::Export* exportApplication = static_cast<Framework::Export*>(mApplication.get());

	if (exportApplication != nullptr)
	{
		exportApplication->writeLoadSbsar(context, message.toUtf8().constData());
	}
}

void ConnectorExport::sendLoadAsset(unsigned int context, const QString& message)
{
	Framework::Export* exportApplication = static_cast<Framework::Export*>(mApplication.get());

	if (exportApplication != nullptr)
	{
		exportApplication->writeLoadAsset(context, message.toUtf8().constData());
	}
}
} // namespace Qt
} // namespace Connector
} // namespace Substance
