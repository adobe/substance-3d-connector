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
//! @brief Base application type for live sbsar update and import
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#include <substance/connector/types.h>
#include <substance/connector/framework/export.h>
#include <substance/connector/connector.h>
#include <substance/connector/framework/details/sendtoschema.h>
#include <substance/connector/framework/details/dynamicload.h>
#include <json/value.h>
#include <json/writer.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
const substance_connector_uuid_t Export::sLoadSbsarId = {
	// 72538d04-276f-4254-a45b-d3654f705477
	0x72538d04u,
	0x276f4254u,
	0xa45bd365u,
	0x4f705477u};

const substance_connector_uuid_t Export::sUpdateSbsarId = {
	// 1643e0bf-7344-4314-9cbf-6107e30e5ed7
	0x1643e0bfu,
	0x73444314u,
	0x9cbf6107u,
	0xe30e5ed7u};

const substance_connector_uuid_t Export::sLoadAssetId = {
	// 91e3dfbc-80b8-4b1a-92d5-63ec09ac641a
	0x91e3dfbcu,
	0x80b84b1au,
	0x92d563ecu,
	0x09ac641au};

Export::Export() : mRecvLoadSbsar(nullptr), mRecvLoadAsset(nullptr), mRecvUpdateSbsar(nullptr)
{
}

Export::~Export()
{
}

void Export::preInit()
{
	// Initialize the callbacks
	mCallbacks.push_back(CallbackPair(sLoadSbsarId, {mRecvLoadSbsar}));
	mCallbacks.push_back(CallbackPair(sLoadAssetId, {mRecvLoadAsset}));
	mCallbacks.push_back(CallbackPair(sUpdateSbsarId, {mRecvUpdateSbsar}));

	Application::preInit();
}

void Export::postInit()
{
}

void Export::postShutdown()
{
}

const std::vector<substance_connector_uuid_t> Export::getFeatureIds() {
	return {
	  Export::sLoadSbsarId,
	  Export::sUpdateSbsarId,
	  Export::sLoadAssetId,
	};
}

void Export::writeLoadSbsar(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sLoadSbsarId, message);
}

void Export::writeLoadAsset(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sLoadAssetId, message);
}

void Export::writeLoadAsset(unsigned int context, Details::send_to_schema& schema)
{
	// Transform the schemea to a json value
	Json::Value json;
	schema.Serialize(json);

	// Build the json strirng
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	const std::string output = Json::writeString(builder, json);

	writeLoadSbsar(context, output.c_str());
}

} // namespace Framework
} // namespace Connector
} // namespace Substance
