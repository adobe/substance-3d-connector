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

//! @file export.h
//! @brief Base application type for live asset update and import
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/framework/application.h>
#include <substance/connector/framework/details/sendtoschema.h>
#include <vector>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class Export : public Application
{
public:
	//! @brief Default constructor
	Export();

	//! @brief Destructor
	virtual ~Export();

	//! @brief Run live export preinitialization steps
	void preInit() override;

	//! @brief Run live export postinitialization
	void postInit() override;

	//! @brief Run live export postshutdown
	void postShutdown() override;

	//! @brief Returns the list of feature ids for this application
	const std::vector<substance_connector_uuid_t> getFeatureIds() override;

	//! @brief Write an export message to a given connection
	void writeLoadSbsar(unsigned int context, const char* message);

	//! @brief Write an export message to a given connection
	void writeLoadAsset(unsigned int context, const char* message);

	//! @brief Write an export message to a given connection
	void writeLoadAsset(unsigned int context, Details::send_to_schema& schema);

public:
	static const substance_connector_uuid_t sLoadSbsarId;
	static const substance_connector_uuid_t sLoadAssetId;
	static const substance_connector_uuid_t sUpdateSbsarId;

	MessageCallback mRecvLoadSbsar;
	MessageCallback mRecvLoadAsset;
	MessageCallback mRecvUpdateSbsar;

	size_t mUserData;
};
} // namespace Framework
} // namespace Connector
} // namespace Substance
