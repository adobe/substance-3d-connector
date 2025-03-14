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

//! @file sendmesh.h
//! @brief QObject implementation of the send mesh to interface
//! @author Daniel Stover - Adobe
//! @date 20241107
//! @copyright Adobe. All rights reserved.

#include <substance/connector/qt/application.h>
#include <substance/connector/framework/schemas/sendtoschema.h>
#include <substance/connector/framework/schemas/sendmeshschema.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
class MeshApplication : public ConnectorApplication
{
	Q_OBJECT

public:
	MeshApplication(QObject* parent = nullptr);

	virtual ~MeshApplication();

	//! @brief Set up the internal state for this implementation
	void initialize() override;

	//! @brief Tear down the internal state
	void shutdown() override;

	//! @brief Send another process an event to load a given mesh
	//! @param context Integer representing the underlying connection
	//! @param string representation of a send_to_schema
	void sendLoadMesh(unsigned int context, const QString& message);

	//! @brief Send another process an event to load a given mesh
	//! @param context Integer representing the underlying connection
	//! @param schema instance of a send_to_schema for sending an import event
	void sendLoadMesh(unsigned int context, Framework::Schemas::send_to_schema& schema);

	//! @brief Send another process an event to request the expected mesh export config
	//! @param context Integer representing the underlying connection
	//! @param message string representation of send_mesh_config_request_schema
	void sendRequestMeshConfig(unsigned int context, const QString& message);

	//! @brief Send another process an event to request the expected mesh export config
	//! @param context Integer representing the underlying connection
	//! @param schema schema instance representing asset details for requesting export config
	void sendRequestMeshConfig(unsigned int context,
							   Framework::Schemas::send_mesh_config_request_schema& schema);

	//! @brief Send exporter the preferred mesh export config in response to receiving a request event
	//! @param context Integer representing the underlying connection id
	//! @param message string representation of mesh_export_schema
	void sendMeshExportConfig(unsigned int context, const QString& message);

	//! @brief Send exporter the preferred mesh export config in response to receiving a request event
	//! @param context Integer representing the underlying connection id
	//! @param schema schema instance representing expected mesh export configuration
	void sendMeshExportConfig(unsigned int context, Framework::Schemas::mesh_export_schema& schema);

Q_SIGNALS:
	//! @brief event for when another application is sending an import event
	//! @param context Integer representing the underlying connection
	//! @param message string representation of a send_to_schema
	void recvLoadMesh(unsigned int context, const QString& message);

	//! @brief event for when an exporting application is requesting information regarding how it should
	//! export a mesh
	//! @param context Integer representing the underlying connection
	//! @param message string representation of a send_mesh_config_request_schema
	void mRecvMeshConfigRequest(unsigned int context, const QString& message);

	//! @brief event for when an importing application responds with expected export config
	//! @param context Integer representing the underlying connection
	//! @param message string representation of a mesh_export_schema
	void recvMeshExportConfig(unsigned int context, const QString& message);
};

} // namespace Qt
} // namespace Connector
} // namespace Substance
