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

//! @file module.h
//! @brief Qt interface for the connector framework
//! @author Galen Helfter - Adobe
//! @date 20190607
//! @copyright Adobe. All rights reserved.

#pragma once

#include <QObject>

namespace Substance
{
namespace Connector
{
namespace Qt
{
class ConnectorApplication;

class ConnectorModule : public QObject
{
	Q_OBJECT

public:
	virtual ~ConnectorModule() = default;

	//! @brief Query the version number of the connector library
	//! @return Semantic version number as a string
	QString version() const;

	//! @brief Initialize the connector library
	//! @param applicationName String identifier of this application
	//! @param module Pointer to the loaded module of the library, only used
	//!               for dynamic loading.
	//! @return True on success, false on failure
	bool init(const std::string& applicationName, void* module = nullptr);

	//! @brief Shut down the connector library
	//! @return True on success, false on failure
	bool shutdown();

	//! @brief Register an application with the core systems
	//! @param application Pointer to a ConnectorApplication QObject
	//! @return True on success, false on failure
	bool registerApplication(ConnectorApplication* application);

	//! @brief Open a TCP connection at the given port
	//! @param port Integer port to open the connection at
	//! @param context Pointer to return the context through
	//! @return True on success, false on failure
	bool openTcpConnection(unsigned int port, unsigned int* context);

	//! @brief Open a default TCP connection
	//! @param context Pointer to return the context through
	//! @return True on success, false on failure
	bool openDefaultTcp(unsigned int* context);

	//! @brief Broadcast connect using tcp connections
	//! @return True on success, false on failure
	bool broadcastTcp();
};
} // namespace Qt
} // namespace Connector
} // namespace Substance
