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

//! @file sendmeshschema.h
//! @brief Outlines the message schema template for send mesh events
//! @author Dan Stover - Adobe
//! @date 20240916
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/connector.h>
#include <string>
#include <optional>
#include <map>
#include <json/value.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Schemas
{
struct mesh_export_schema
{
	mesh_export_schema();

	virtual ~mesh_export_schema();

	/** Functions to convert take from this class to Jsoncpp and vice versa */
	virtual void Serialize(Json::Value& root);
	virtual void Deserialize(const std::string& json);
	virtual void Deserialize(const Json::Value& root);
	virtual std::string GetJsonString();

	enum class ColorEncoding
	{
		Linear,
		SRGB,
	};

	static std::map<ColorEncoding, std::string> ColorEncodingStrings;

	enum class ColorFormat
	{
		None,    // Will ignore any color of the sent mesh
		Vertex,  // Will apply vertex colors if sent
		Texture, // Will accept materials and textures if sent
	};

	static std::map<ColorFormat, std::string> ColorFormatStrings;

	enum class FileFormat
	{
		fbx = 0,
		glb,
		gltf,
		igs,
		jt,
		obj,
		step,
		stl,
		stp,
		usd,
		usda,
		usdz,
		usdc,
	};

	static std::map<FileFormat, std::string> FileFormatStrings;

	enum class AxisConvention
	{
		ForceYUp_XRight_mZForward,
		ForceZUp_XRight_YForward,
	};

	static std::map<AxisConvention, std::string> AxisConventionStrings;

	enum class Unit
	{
		Millimeters,
		Centimeters,
		Meters,
		Kilometers,
		Inches,
		Feet,
		Yards,
		Miles,
	};

	static std::map<Unit, std::string> UnitStrings;

	enum class ExportTopology
	{
		Triangles,
		Quads,
	};

	static std::map<ExportTopology, std::string> ExportTopologyStrings;

	// Name of the asset. This should be sent as part of the config request that is to be sent back for
	// context. This value is not optional as part of the request so it is not optional here.
	std::string assetName;

	// Unique id of the asset. This should be sent as part of the send_mesh_config_request_schema that is
	// added here to prevent losing context
	std::optional<std::string> assetUuid;

	// List of supported formats that can be imported in order of preference
	std::optional<std::vector<FileFormat>> supportedFormats;

	// Color Format
	std::optional<ColorFormat> colorFormat;

	// Color Encoding
	std::optional<ColorEncoding> colorEncoding;

	// Hierarchy
	std::optional<bool> allowNegativeTransforms;
	std::optional<bool> allowInstances;
	std::optional<bool> flattenHierarchy;

	// Orientation/Units
	std::optional<AxisConvention> axisConvention;
	std::optional<Unit> unit;

	// Topology
	std::optional<ExportTopology> topology;  // types of polygons expected
	// std::optional<uint32_t> targetFaceCount; // this should eventually evolve into a range but this is simple
											 // and to the point. If specified, use a reasonable poly count,
											 // don't request zero

	std::optional<bool> requestUv;
	std::optional<bool> enableUdims;

}; // send_mesh_schema

// A struct that will be used for the inital config request.
struct send_mesh_config_request_schema
{
	send_mesh_config_request_schema();

	virtual ~send_mesh_config_request_schema();

	// Name or path of the asset?
	std::string assetName;

	// A uuid in the case the name isn't enough or for explicitly setting an asset?
	std::optional<std::string> assetUuid;

	/** Functions to convert take from this class to Jsoncpp and vice versa */
	virtual void Serialize(Json::Value& root);
	virtual void Deserialize(const std::string& json);
	virtual void Deserialize(const Json::Value& root);
	virtual std::string GetJsonString();

}; // send_mesh_config_request

} // namespace Schemas
} // namespace Framework
} // namespace Connector
} // namespace Substance