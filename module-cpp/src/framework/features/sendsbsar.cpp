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

//! @file sendsbsar.h
//! @brief Base application type for sbsar update and import
//! @author Daniel Stover - Adobe
//! @date 20240828
//! @copyright Adobe. All rights reserved.

#include <substance/connector/types.h>
#include <substance/connector/connector.h>
#include <substance/connector/framework/features/sendsbsar.h>
#include <substance/connector/framework/schemas/sendtoschema.h>
#include <substance/connector/framework/details/dynamicload.h>
#include <json/value.h>
#include <json/writer.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
const substance_connector_uuid_t SbsarApplication::sLoadSbsarId = {
	// 91e3dfbc-80b8-4b1a-92d5-63ec09ac641a
	0x91e3dfbcu,
	0x80b84b1au,
	0x92d563ecu,
	0x09ac641au};

const substance_connector_uuid_t SbsarApplication::sUpdateSbsarId = {
	// 1643e0bf-7344-4314-9cbf-6107e30e5ed7
	0x1643e0bfu,
	0x73444314u,
	0x9cbf6107u,
	0xe30e5ed7u};


SbsarApplication::SbsarApplication() : mRecvLoadSbsar(nullptr), mRecvUpdateSbsar(nullptr)
{
}

SbsarApplication::~SbsarApplication()
{
}

void SbsarApplication::preInit()
{
	// Initialize the callbacks
	mCallbacks.push_back(CallbackPair(sLoadSbsarId, {mRecvLoadSbsar}));
	mCallbacks.push_back(CallbackPair(sUpdateSbsarId, {mRecvUpdateSbsar}));

	Application::preInit();
}

void SbsarApplication::postInit()
{
}

void SbsarApplication::postShutdown()
{
}

const std::vector<substance_connector_uuid_t> SbsarApplication::getFeatureIds() {
	return {
	  SbsarApplication::sLoadSbsarId,
	  SbsarApplication::sUpdateSbsarId,
	};
}

void SbsarApplication::sendLoadSbsar(unsigned int context, const char* message)
{
	CONNECTOR_FRAMEWORK_CALL(write_message)(context, &sLoadSbsarId, message);
}

void SbsarApplication::sendLoadSbsar(unsigned int context, Schemas::send_to_schema& schema)
{
	// Transform the schema to a json value
	Json::Value json;
	schema.Serialize(json);

	// Build the json string
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	const std::string output = Json::writeString(builder, json);

	sendLoadSbsar(context, output.c_str());
}

} // namespace Framework
} // namespace Connector
} // namespace Substance
