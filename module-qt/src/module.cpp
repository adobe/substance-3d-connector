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

//! @file module.cpp
//! @brief Qt interface for the connector framework
//! @author Galen Helfter - Adobe
//! @date 20190607
//! @copyright Adobe. All rights reserved.

#include <substance/connector/qt/module.h>
#include <substance/connector/qt/application.h>
#include <substance/connector/connector.h>
#include <substance/connector/errorcodes.h>
#include <substance/connector/framework/core.h>
#include <substance/connector/framework/application.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
QString ConnectorModule::version() const
{
	return QString(Framework::version());
}

bool ConnectorModule::init(const std::string& applicationName, void* module)
{
	return Framework::init(applicationName, module);
}

bool ConnectorModule::shutdown()
{
	return Framework::shutdown();
}

bool ConnectorModule::registerApplication(ConnectorApplication* application)
{
	return Framework::registerApplication(application->mApplication.get());
}

bool ConnectorModule::openTcpConnection(unsigned int port, unsigned int* context)
{
	return Framework::openTcpConnection(port, context);
}

bool ConnectorModule::openDefaultTcp(unsigned int* context)
{
	return Framework::openDefaultTcp(context);
}

bool ConnectorModule::broadcastTcp()
{
	return Framework::broadcastTcp();
}
} // namespace Qt
} // namespace Connector
} // namespace Substance
