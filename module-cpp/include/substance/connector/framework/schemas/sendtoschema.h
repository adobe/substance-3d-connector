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

//! @file sendtoschema.h
//! @brief Outlines the message schema template for send to events
//! @author Dan Stover - Adobe
//! @date 2023420
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/connector.h>
#include <string>
#include <json/value.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Schemas
{
struct send_to_schema
{
	virtual ~send_to_schema() = default;

	/** Functions to convert take from this class to Jsoncpp and vice versa */
	virtual void Serialize(Json::Value& root);
	virtual void Deserialize(const std::string& json);
	virtual void Deserialize(const Json::Value& root);
	virtual std::string getPathExtension();
	virtual bool isPathFileExtension(const std::string& extension);

	/** Helper function to return a string representing the json */
	std::string GetJsonString();

	/** Specifies whether the receiving application should now own the asset at path */
	bool take_file_ownership;

	/** Path of the asset from the send to event */
	std::string path;

	/** UI name of the asset being sent */
	std::string name;

	/** Asset unique id */
	std::string uuid;

	/** The type of asset being sent */
	std::string type;
};

} // namespace Schemas
} // namespace Framework
} // namespace Connector
} // namespace Substance
