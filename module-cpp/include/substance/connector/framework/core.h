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

//! @file core.h
//! @brief Defines core operations for Substance Connector
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#pragma once
#include<string>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class Application;

//! @brief Acquire the version of the Substance Connector library
//! @return Pointer to the string representation of the version
const char* version();

//! @brief Register an application with the core system
//! @param application Pointer to a persistent application object
//! @return True on success, false on failure. Fails if the application
//          is already registered or if the system is already initialized.
bool registerApplication(Application* application);

//! @brief Initialize the core connector system
//! @param applicationName String identifier for this application
//! @param module Pointer to the dynamically loaded library
//! @return True on success, false on failure
bool init(const std::string& applicationName, void* module = nullptr);

//! @brief Initialize the core connector system
//! @param applicationName String display name for this application
//! @param applicationId String identifier for this application
//! @param module Pointer to the dynamically loaded library
//! @return True on success, false on failure
bool init(const std::string& applicationName, const std::string& applicationId, void* module = nullptr);

//! @brief Shut down the core connector systems
//! @return True on success, false on failure
bool shutdown();

//! @brief Open a TCP connection at the given port
//! @param port Integer port to open the connection at
//! @param context Pointer to return the context through
//! @return True on success, false on failure
bool openTcpConnection(unsigned int port, unsigned int* context);

//! @brief Establish a TCP connection at the port
//! @param port Integer port to open the connection at
//! @param context Pointer to return the context through
//! @return True on success, false on failure
bool establishTcpConnection(unsigned int port, unsigned int* context);

//! @brief Open a default TCP connection
//! @param[out] context Pointer to return the context throughh
//! @return True on success, false on failure
bool openDefaultTcp(unsigned int* context);

//! @brief Perform a broadcast tcp connection, finding other connector instances
//! @return True on success, false on failure
bool broadcastTcp();

//! @brief Open a default unix connection
//! @param[out] context Pointer to return the context throughh
//! @return True on success, false on failure
bool openDefaultUnix(unsigned int* context);

//! @brief Perform a broadcast unix connection, finding other connector instances
//! @return True on success, false on failure
bool broadcastUnix();

//! @brief Open a default, system-specific connection
//! @param[out] context Pointer to return the context throughh
//! @return True on success, false on failure
bool openDefault(unsigned int* context);

//! @brief Perform a default broadcast connection, finding other connector instances
//! @return True on success, false on failure
bool broadcastDefault();

//! @brief Provides the connection context describing this connection
//! @return connetion_schema with the context for this connection as a string
std::string getConnectionContext();
} // namespace Framework
} // namespace Connector
} // namespace Substance
