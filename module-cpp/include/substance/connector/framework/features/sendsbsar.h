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

#pragma once

#include <substance/connector/framework/application.h>
#include <substance/connector/framework/schemas/sendtoschema.h>
#include <vector>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class SbsarApplication : public Application
{
public:
	//! @brief Default constructor
	SbsarApplication();

	//! @brief Destructor
	virtual ~SbsarApplication();

	//! @brief Run live SbsarApplication preinitialization steps
	void preInit() override;

	//! @brief Run live SbsarApplication postinitialization
	void postInit() override;

	//! @brief Run live SbsarApplication postshutdown
	void postShutdown() override;

	//! @brief Returns the list of feature ids for this application
	const std::vector<substance_connector_uuid_t> getFeatureIds() override;

	//! @brief Send to another connection a message to load an sbsar
	//! @param context Integer representing the underlying connection
	//! @param message File path of the sbsar for the other application to load
	void sendLoadSbsar(unsigned int context, const char* message);

	//! @brief Send to another application a message to load the sbsar
	//! @param context Integer representing the underlying connection
	//! @param message File path of the sbsar for the other application to load
	void sendLoadSbsar(unsigned int context, Schemas::send_to_schema& message);

public:
	static const substance_connector_uuid_t sLoadSbsarId;
	static const substance_connector_uuid_t sUpdateSbsarId;

	MessageCallback mRecvLoadSbsar;
	MessageCallback mRecvUpdateSbsar;

	size_t mUserData;
};
} // namespace Framework
} // namespace Connector
} // namespace Substance
