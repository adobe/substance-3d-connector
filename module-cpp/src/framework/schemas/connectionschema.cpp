/*
Copyright 2023 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include <substance/connector/framework/schemas/connectionschema.h>
#include <substance/connector/framework/uuid.h>
#include <json/reader.h>
#include <json/writer.h>
#include <algorithm>
#include <iomanip>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Schemas
{
void connection_schema::Serialize(Json::Value& root)
{
	root["display_name"] = display_name;
	root["id_name"] = id_name;
	root["connector_version"] = connector_version;
	Json::Value featureValue;
	for (const auto& feature : available_features)
	{
		featureValue.append(uuidToString(feature));
	}
	root["available_features"] = featureValue;
}

void connection_schema::Deserialize(const std::string& json)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	const auto jsonLength = static_cast<int>(json.length());
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	JSONCPP_STRING err;
	if (!reader->parse(json.c_str(), json.c_str() + jsonLength, &root, &err))
	{
		// #TODO:: Report error
	}
	else
	{
		Deserialize(root);
	}
}

void connection_schema::Deserialize(const Json::Value& root)
{
	display_name = root.get("display_name", "").asString();
	id_name = root.get("id_name", "").asString();
	connector_version = root.get("connector_version", "").asString();
	std::vector<substance_connector_uuid_t> featuresIds;
	const Json::Value featureArray = root["available_features"];
	featuresIds.reserve(featureArray.size());
	std::transform(featureArray.begin(),
				   featureArray.end(),
				   std::back_inserter(featuresIds),
				   [](const Json::Value& v) -> substance_connector_uuid_t
				   { return uuidFromString(v.asString()); });
	available_features = std::move(featuresIds);
}

std::string connection_schema::GetJsonString()
{
	Json::Value value;
	Serialize(value);
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	std::string value_string = Json::writeString(builder, value);
	return std::move(value_string);
}

} // namespace Schemas
} // namespace Framework
} // namespace Connector
} // namespace Substance
