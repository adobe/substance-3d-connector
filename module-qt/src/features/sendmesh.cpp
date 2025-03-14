/*
Copyright 2025 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

//! @file sendmesh.cpp
//! @brief QObject implementation of the send single mesh to interface
//! @author Daniel Stover - Adobe
//! @date 20250123
//! @copyright Adobe. All rights reserved.

#include <substance/connector/qt/features/sendmesh.h>
#include <substance/connector/framework/features/sendmesh.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{

// Singleton instance of this application
static std::atomic<MeshApplication*> instance(nullptr);

// Static callbacks for signal/slots to call into
static void recvLoadMeshCallback(unsigned int context,
								 const substance_connector_uuid_t* uuid,
								 const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvLoadMesh(context, QString(message));
	}
}

static void recvRequestMeshConfigCallback(unsigned int context,
										  const substance_connector_uuid_t* uuid,
										  const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->mRecvMeshConfigRequest(context, QString(message));
	}
}

static void recvMeshConfigCallback(unsigned int context,
								   const substance_connector_uuid_t* uuid,
								   const char* message)
{
	if (instance.load() != nullptr)
	{
		Q_EMIT instance.load()->recvMeshExportConfig(context, QString(message));
	}
}

MeshApplication::MeshApplication(QObject* parent) : ConnectorApplication(parent)
{
}

MeshApplication::~MeshApplication()
{
}

void MeshApplication::initialize()
{
	MeshApplication* expected = nullptr;

	// Only a single instance should be valid at a given time
	if (instance.compare_exchange_strong(expected, this))
	{
		// Instantiate a new instance of the base framework MeshApplication
		// application type
		mApplication = std::make_unique<Framework::MeshApplication>();

		Framework::MeshApplication* meshApplication =
			static_cast<Framework::MeshApplication*>(mApplication.get());

		// Update the callback pointers
		meshApplication->mRecvMeshConfigRequest = recvRequestMeshConfigCallback;
		meshApplication->mRecvMeshConfig = recvMeshConfigCallback;
		meshApplication->mRecvLoadMesh = recvLoadMeshCallback;
	}
}

void MeshApplication::shutdown()
{
	if (instance.load() == this)
	{
		mApplication = nullptr;

		// Only clear the atomic pointer after the shutdown has been completed
		MeshApplication* expected = this;
		instance.compare_exchange_strong(expected, nullptr);
	}
}

void MeshApplication::sendLoadMesh(unsigned int context, const QString& message)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendLoadMesh(context, message.toUtf8().constData());
	}
}

void MeshApplication::sendLoadMesh(unsigned int context, Framework::Schemas::send_to_schema& schema)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendLoadMesh(context, schema);
	}
}

void MeshApplication::sendRequestMeshConfig(unsigned int context, const QString& message)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendRequestMeshConfig(context, message.toUtf8().constData());
	}
}

void MeshApplication::sendRequestMeshConfig(unsigned int context,
											Framework::Schemas::send_mesh_config_request_schema& schema)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendRequestMeshConfig(context, schema);
	}
}

void MeshApplication::sendMeshExportConfig(unsigned int context, const QString& message)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendMeshExportConfig(context, message.toUtf8().constData());
	}
}

void MeshApplication::sendMeshExportConfig(unsigned int context,
										   Framework::Schemas::mesh_export_schema& schema)
{
	Framework::MeshApplication* meshApplication =
		static_cast<Framework::MeshApplication*>(mApplication.get());

	if (meshApplication != nullptr)
	{
		meshApplication->sendMeshExportConfig(context, schema);
	}
}

} // namespace Qt
} // namespace Connector
} // namespace Substance
