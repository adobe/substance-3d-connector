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

//! @file sendmesh.cpp
//! @brief Base application type for generic mesh update and import
//! @author Daniel Stover - Adobe
//! @date 20240916
//! @copyright Adobe. All rights reserved.

#include <json/value.h>
#include <json/writer.h>
#include <substance/connector/types.h>
#include <substance/connector/connector.h>
#include <substance/connector/framework/features/sendmesh.h>
#include <substance/connector/framework/details/dynamicload.h>
#include <substance/connector/framework/schemas/sendmeshschema.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
const substance_connector_uuid_t MeshApplication::sLoadMeshId = {
	// fad67b5b-e141-4be7-b27a-d6f6a4f89424
	0xfad67b5bu,
	0xe1414be7u,
	0xb27ad6f6u,
	0xa4f89424u};

const substance_connector_uuid_t MeshApplication::sUpdateMeshId = {
	// 7c3e3c65-0c16-41d5-808b-953759930ffe
	0x7c3e3c65u,
	0x0c1641d5u,
	0x808b9537u,
	0x59930ffeu};

const substance_connector_uuid_t MeshApplication::sRequestMeshConfigId = {
	// 685dd700-9d31-455d-98a4-431bb039819a
	0x685dd700u,
	0x9d31455du,
	0x98a4431bu,
	0xb039819au};

const substance_connector_uuid_t MeshApplication::sRecMeshConfigId = {
	// 0b6d7b54-a90b-4c05-b0f9-359271b2efbc
	0x0b6d7b54u,
	0xa90b4c05u,
	0xb0f93592u,
	0x71b2efbcu};

MeshApplication::MeshApplication()
	: mRecvLoadMesh(nullptr)
	, mRecvMeshConfig(nullptr)
	, mRecvUpdateMesh(nullptr)
	, mRecvMeshConfigRequest(nullptr)
{
}

MeshApplication::~MeshApplication()
{
}

void MeshApplication::preInit()
{
	// Initialize the callbacks
	mCallbacks.push_back(CallbackPair(sLoadMeshId, {mRecvLoadMesh}));
	mCallbacks.push_back(CallbackPair(sUpdateMeshId, {mRecvUpdateMesh}));
	mCallbacks.push_back(CallbackPair(sRecMeshConfigId, {mRecvMeshConfig}));
	mCallbacks.push_back(CallbackPair(sRequestMeshConfigId, {mRecvMeshConfigRequest}));

	Application::preInit();
}

void MeshApplication::postInit()
{
}

void MeshApplication::postShutdown()
{
}

const std::vector<substance_connector_uuid_t> MeshApplication::getFeatureIds()
{
	return {
		MeshApplication::sLoadMeshId,
		MeshApplication::sUpdateMeshId,
		MeshApplication::sRecMeshConfigId,
		MeshApplication::sRequestMeshConfigId,
	};
}

void MeshApplication::sendRequestMeshConfig(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sRequestMeshConfigId, message);
}

void MeshApplication::sendMeshExportConfig(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sRecMeshConfigId, message);
}

void MeshApplication::sendLoadMesh(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sLoadMeshId, message);
}

void MeshApplication::sendRequestMeshConfig(unsigned int context,
											Schemas::send_mesh_config_request_schema& schema)
{
	// Transform the schema to a json value
	Json::Value json;
	schema.Serialize(json);

	// Build the json string
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	const std::string output = Json::writeString(builder, json);

	sendRequestMeshConfig(context, output.c_str());
}

void MeshApplication::sendLoadMesh(unsigned int context, Schemas::send_to_schema& schema)
{
	// Transform the schema to a json value
	Json::Value json;
	schema.Serialize(json);

	// Build the json string
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	const std::string output = Json::writeString(builder, json);

	sendLoadMesh(context, output.c_str());
}

void MeshApplication::sendMeshExportConfig(unsigned int context, Schemas::mesh_export_schema& schema)
{
	// Transform the schema to a json value
	Json::Value json;
	schema.Serialize(json);

	// Build the json string
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	const std::string output = Json::writeString(builder, json);

	sendMeshExportConfig(context, output.c_str());
}

} // namespace Framework
} // namespace Connector
} // namespace Substance
