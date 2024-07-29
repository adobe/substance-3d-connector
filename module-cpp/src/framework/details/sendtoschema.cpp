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

#include <substance/connector/framework/details/sendtoschema.h>
#include <json/reader.h>
#include <json/writer.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{
void send_to_schema::Serialize(Json::Value& root)
{
	root["path"] = path;
	root["name"] = name;
	root["uuid"] = uuid;
	root["type"] = type;
	root["take_file_ownership"] = take_file_ownership;
}

void send_to_schema::Deserialize(const std::string& json)
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

void send_to_schema::Deserialize(const Json::Value& root)
{
	path = root.get("path", "").asString();
	name = root.get("name", "").asString();
	uuid = root.get("uuid", "").asString();
	type = root.get("type", "").asString();
	take_file_ownership = root.get("take_file_ownership", false).asBool();
}

std::string send_to_schema::GetJsonString()
{
	Json::Value value;
	Serialize(value);
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	std::string value_string = Json::writeString(builder, value);
	return std::move(value_string);
}

std::string send_to_schema::getPathExtension()
{
	std::string fileExtension = "";
	if (path.length() > 0)
	{
		fileExtension = path.substr(path.find_last_of(".") + 1);
	}
	return std::move(fileExtension);
}

bool send_to_schema::isPathFileExtension(const std::string& extension)
{
	return this->getPathExtension().compare(extension) == 0;
}

} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
