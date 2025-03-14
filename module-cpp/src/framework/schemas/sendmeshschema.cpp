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

#include <substance/connector/framework/schemas/sendmeshschema.h>
#include <json/reader.h>
#include <json/writer.h>

#include <memory>
#include <algorithm>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Schemas
{

std::map<mesh_export_schema::ColorEncoding, std::string> mesh_export_schema::ColorEncodingStrings = {
	{mesh_export_schema::ColorEncoding::Linear, "Linear"},
	{mesh_export_schema::ColorEncoding::SRGB, "SRGB"},
};

std::map<mesh_export_schema::ColorFormat, std::string> mesh_export_schema::ColorFormatStrings = {
	{mesh_export_schema::ColorFormat::None, "None"},
	{mesh_export_schema::ColorFormat::Vertex, "Vertex"},
	{mesh_export_schema::ColorFormat::Texture, "Texture"},
};

std::map<mesh_export_schema::FileFormat, std::string> mesh_export_schema::FileFormatStrings = {
	{mesh_export_schema::FileFormat::fbx, "fbx"},
	{mesh_export_schema::FileFormat::glb, "glb"},
	{mesh_export_schema::FileFormat::gltf, "gltf"},
	{mesh_export_schema::FileFormat::igs, "igs"},
	{mesh_export_schema::FileFormat::jt, "jt"},
	{mesh_export_schema::FileFormat::obj, "obj"},
	{mesh_export_schema::FileFormat::step, "step"},
	{mesh_export_schema::FileFormat::stl, "stl"},
	{mesh_export_schema::FileFormat::stp, "stp"},
	{mesh_export_schema::FileFormat::usd, "usd"},
	{mesh_export_schema::FileFormat::usda, "usda"},
	{mesh_export_schema::FileFormat::usdz, "usdz"},
	{mesh_export_schema::FileFormat::usdc, "usdc"},
};


std::map<mesh_export_schema::AxisConvention, std::string> mesh_export_schema::AxisConventionStrings = {
	{mesh_export_schema::AxisConvention::ForceYUp_XRight_mZForward, "ForceYUp_XRight_mZForward"},
	{mesh_export_schema::AxisConvention::ForceZUp_XRight_YForward, "ForceZUp_XRight_YForward"},
};

std::map<mesh_export_schema::Unit, std::string> mesh_export_schema::UnitStrings = {
	{mesh_export_schema::Unit::Millimeters, "Millimeters"},
	{mesh_export_schema::Unit::Centimeters, "Centimeters"},
	{mesh_export_schema::Unit::Meters, "Meters"},
	{mesh_export_schema::Unit::Kilometers, "Kilometers"},
	{mesh_export_schema::Unit::Inches, "Inches"},
	{mesh_export_schema::Unit::Feet, "Feet"},
	{mesh_export_schema::Unit::Yards, "Yards"},
	{mesh_export_schema::Unit::Miles, "Miles"},
};

std::map<mesh_export_schema::ExportTopology, std::string> mesh_export_schema::ExportTopologyStrings = {
	{mesh_export_schema::ExportTopology::Triangles, "Triangles"},
	{mesh_export_schema::ExportTopology::Quads, "Quads"},
};

mesh_export_schema::mesh_export_schema()
{
}

mesh_export_schema::~mesh_export_schema()
{
}

void mesh_export_schema::Serialize(Json::Value& root)
{
	root["assetName"] = assetName;

	if (assetUuid.has_value())
	{
		root["assetUuid"] = assetUuid.value();
	}

	if (supportedFormats.has_value())
	{
		for (const auto& itr : supportedFormats.value())
		{
			root["supportedFormats"].append(FileFormatStrings[itr]);
		}
	}

	if (colorFormat.has_value())
	{
		root["colorFormat"] = ColorFormatStrings[colorFormat.value()];
	}

	if (colorEncoding.has_value())
	{
		root["colorEncoding"] = ColorEncodingStrings[colorEncoding.value()];
	}

	if (allowNegativeTransforms.has_value())
	{
		root["allowNegativeTransforms"] = allowNegativeTransforms.value();
	}

	if (allowInstances.has_value())
	{
		root["allowInstances"] = allowInstances.value();
	}

	if (flattenHierarchy.has_value())
	{
		root["flattenHierarchy"] = flattenHierarchy.value();
	}

	if (axisConvention.has_value())
	{
		root["axisConvention"] = AxisConventionStrings[axisConvention.value()];
	}

	if (unit.has_value())
	{
		root["unit"] = UnitStrings[unit.value()];
	}

	if (topology.has_value())
	{
		root["topology"] = ExportTopologyStrings[topology.value()];
	}

	// if (targetFaceCount.has_value())
	// {
	// 	root["targetFaceCount"] = std::to_string(targetFaceCount.value());
	// }

	if (enableUdims.has_value())
	{
		root["enableUdims"] = enableUdims.value();
	}
	if (requestUv.has_value())
	{
		root["requestUv"] = requestUv.value();
	}
}

void mesh_export_schema::Deserialize(const std::string& json)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	const auto jsonLength = static_cast<int>(json.length());
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	JSONCPP_STRING err;
	if (reader->parse(json.c_str(), json.c_str() + jsonLength, &root, &err))
	{
		Deserialize(root);
	}
}

template <typename T>
const T& getKeyFromValue(const std::map<T, std::string>& valueMap, const std::string& value)
{
	auto findResult =
		std::find_if(std::begin(valueMap),
					 std::end(valueMap),
					 [&](const std::pair<T, std::string>& pair) { return pair.second == value; });
	return (*findResult).first;
}

void mesh_export_schema::Deserialize(const Json::Value& root)
{
	assetName = root.get("assetName", "").asString();

	if (root.isMember("assetUuid"))
	{
		assetUuid = root.get("assetUuid", "").asString();
	}

	if (root.isMember("supportedFormats"))
	{
		const auto& formats_array = root.get("supportedFormats", {});
		if ((*formats_array.begin()).asString().size() > 0)
		{
			const auto last_index = formats_array.end().index();
			supportedFormats = {};
			for (Json::UInt index = 0; index <= last_index; index++)
			{
				mesh_export_schema::FileFormat current_value =
					getKeyFromValue(FileFormatStrings, formats_array[index].asString());
				supportedFormats.value().emplace_back(current_value);
			}
		}
	}

	if (root.isMember("colorFormat"))
	{
		colorFormat = getKeyFromValue(ColorFormatStrings, root.get("colorFormat", "").asString());
	}

	if (root.isMember("colorEncoding"))
	{
		colorEncoding = getKeyFromValue(ColorEncodingStrings, root.get("colorEncoding", "").asString());
	}

	if (root.isMember("allowInstances"))
	{
		allowInstances = root.get("allowInstances", "").asBool();
	}

	if (root.isMember("flattenHierarchy"))
	{
		flattenHierarchy = root.get("flattenHierarchy", "").asBool();
	}

	if (root.isMember("allowNegativeTransforms"))
	{
		allowNegativeTransforms = root.get("allowNegativeTransforms", "").asBool();
	}

	if (root.isMember("unit"))
	{
		unit = getKeyFromValue(UnitStrings, root.get("unit", "").asString());
	}

	if (root.isMember("axisConvention"))
	{
		axisConvention = getKeyFromValue(AxisConventionStrings, root.get("axisConvention", "").asString());
	}

	if (root.isMember("topology"))
	{
		topology = getKeyFromValue(ExportTopologyStrings, root.get("topology", "").asString());
	}

	// if (root.isMember("targetFaceCount"))
	// {
	// 	targetFaceCount = root.get("targetFaceCount", "").asUInt();
	// }

	if (root.isMember("enableUdims"))
	{
		enableUdims = root.get("enableUdims", "").asBool();
	}

	if (root.isMember("requestUv"))
	{
		requestUv = root.get("requestUv", "").asBool();
	}
}

std::string mesh_export_schema::GetJsonString()
{
	Json::Value value;
	Serialize(value);
	Json::StreamWriterBuilder builder;
	builder["indentation"] = "";
	std::string value_string = Json::writeString(builder, value);
	return std::move(value_string);
}

send_mesh_config_request_schema::send_mesh_config_request_schema()
{
}

send_mesh_config_request_schema::~send_mesh_config_request_schema()
{
}

void send_mesh_config_request_schema::Serialize(Json::Value& root)
{
	root["assetName"] = assetName;

	if (assetUuid.has_value())
	{
		root["assetUuid"] = assetUuid.value();
	}
}

void send_mesh_config_request_schema::Deserialize(const std::string& json)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	const auto jsonLength = static_cast<int>(json.length());
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	JSONCPP_STRING err;
	if (reader->parse(json.c_str(), json.c_str() + jsonLength, &root, &err))
	{
		Deserialize(root);
	}
}

void send_mesh_config_request_schema::Deserialize(const Json::Value& root)
{
	assetName = root.get("assetName", "").asString();

	if (root.isMember("assetUuid"))
	{
		assetUuid = root.get("assetUuid", "").asString();
	}
}

std::string send_mesh_config_request_schema::GetJsonString()
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
