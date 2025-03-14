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
//! @brief Base application type for generic mesh update and import
//! @author Daniel Stover - Adobe
//! @date 20240916
//! @copyright Adobe. All rights reserved.

/******************************************************************************
 * The send mesh feature allows an exporting application to send a mesh file to
 * an importing application. Unlike some of the other workflows, this has additional
 * steps that allow the importing application to provide some context into how
 * it expects an mesh to be exported so that it best works within the importing
 * environment. This context is provided using the send_mesh_config_request_schema
 * class.
 *
 * As an importer, it is recommended that when a new connection context is received,
 * if that connection context has the mRecvMeshConfig event within it's available features
 * that you send it your applications defined send_mesh_config_request_schema using the
 * sendMeshExportConfig functions defined within this class. In the case that the
 * exporter chooses to store the configs once rathe than request per asset.
 *
 * Alternatively, as an exporter, if importers do not send over a
 * send_mesh_config_request_schema, you can request the config from any importer that
 * has the feature bound by calling the sendRequestMeshConfig function defined within
 * this feature class.
 *
 * The sendLoadMesh function works like other send to entry points and does not require the
 * context in order to send an asset. However, bypassing the context does not guarantee
 * that the importer will be able to import any asset that is sent. For instance, if the
 * importer does not support importing USD files but the exporter sends one regardless,
 * the importerr will not import the file and it will appear as though the feature does
 * not work.
 *
 ******************************************************************************/

#pragma once

#include <substance/connector/framework/application.h>
#include <substance/connector/framework/schemas/sendtoschema.h>
#include <substance/connector/framework/schemas/sendmeshschema.h>
#include <vector>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class MeshApplication : public Application
{
public:
	//! @brief Default constructor
	MeshApplication();

	//! @brief Destructor
	virtual ~MeshApplication();

	//! @brief Run live MeshApplication preinitialization steps
	void preInit() override;

	//! @brief Run live MeshApplication postinitialization
	void postInit() override;

	//! @brief Run live MeshApplication postshutdown
	void postShutdown() override;

	//! @brief Returns the list of feature ids for this application
	const std::vector<substance_connector_uuid_t> getFeatureIds() override;

	//! @brief Send an event to another connection to load a mesh
	//! @param context Integer representing the id of the connection to send to
	//! @param message string representation of a send_to_schema instance
	void sendLoadMesh(unsigned int context, const char* message);

	//! @brief Send an event to another connection to load a mesh
	//! @param context Integer representing the id of the connection to send to
	//! @param message schema containing all of the details of the asset to import
	void sendLoadMesh(unsigned int context, Schemas::send_to_schema& schema);

	//! @brief Send an event to another connection respond to a mesh export config request
	//! @param context Integer representing the id of the connection to send to the export configuration to
	//! @param message string representation of a mesh_export_schema instance
	void sendMeshExportConfig(unsigned int context, const char* message);

	//! @brief Send an event to another connection respond to a mesh export config request
	//! @param context Integer representing the id of the connection to send to
	//! @param schema Instance containing all of the config details for how an importing applications expects
	//! a mesh to be exported
	void sendMeshExportConfig(unsigned int context, Schemas::mesh_export_schema& schema);

	//! @brief Send a request the expected mesh export config from an importing application
	//! @param context Integer representing the id of the connection to send to this request to
	//! @param message string representation of a send_mesh_config_request_schema instance
	void sendRequestMeshConfig(unsigned int context, const char* message);

	//! @brief Send a request the expected mesh export config from an importing application
	//! @param context Integer representing the id of the connection to send to this request to
	//! @param message schema instance containing initial context information of the asset that is to be
	//! exported
	void sendRequestMeshConfig(unsigned int context, Schemas::send_mesh_config_request_schema& config_schema);

public:
	static const substance_connector_uuid_t sLoadMeshId;
	static const substance_connector_uuid_t sUpdateMeshId;
	static const substance_connector_uuid_t sRecMeshConfigId;
	static const substance_connector_uuid_t sRequestMeshConfigId;

	MessageCallback mRecvLoadMesh;
	MessageCallback mRecvUpdateMesh;
	MessageCallback mRecvMeshConfig;
	MessageCallback mRecvMeshConfigRequest;

	size_t mUserData;
};
} // namespace Framework
} // namespace Connector
} // namespace Substance
