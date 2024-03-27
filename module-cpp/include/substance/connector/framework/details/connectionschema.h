//! @file connectionschema.h
//! @brief Outlines the connection schema template for established connections
//! @author Dan Stover - Adobe
//! @date 2023918
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/connector.h>
#include <string>
#include <vector>
#include <json/value.h>

namespace Substance
{
namespace Connector
{
namespace Framework
{
namespace Details
{

struct connection_schema
{
  /** Initialize a json object with the values of this classes member variables */
  virtual void Serialize(Json::Value& root);

  /** Given a string representing a json object, assign member variables of this class */
  virtual void Deserialize(const std::string& json);

  /** Given a json object, assign all member variables of this class to the json value */
  virtual void Deserialize(const Json::Value& root);

  /** Returns a string representation of the json for this instance */
  std::string GetJsonString();

  /** String to display within the UI representing the display name of the connection */
  std::string display_name;

  /** String to identify an endpoint */
  std::string id_name;

  /** String value representing the loaded version of the connector lib */
  std::string connector_version;

  /** Map of all of the feature components as part of this connection */
  std::vector<std::string> available_features;
};

} // namespace Details
} // namespace Framework
} // namespace Connector
} // namespace Substance
