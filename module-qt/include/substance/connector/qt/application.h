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

//! @file application.h
//! @brief QObject implementation of an application object
//! @author Galen Helfter - Adobe
//! @date 20190607
//! @copyright Adobe. All rights reserved.

#pragma once

#include <memory>
#include <QObject>

namespace Substance
{
namespace Connector
{
namespace Framework
{
class Application;
} // namespace Framework

namespace Qt
{
class ConnectorApplication : public QObject
{
	Q_OBJECT

public:
	ConnectorApplication(QObject* parent = nullptr);

	virtual ~ConnectorApplication();

	//! @brief Setup the application's initial state
	virtual void initialize() = 0;

	//! @brief Close the application's state on shutdown
	virtual void shutdown() = 0;

protected:
	//! @brief Internal pointer to the application
	std::unique_ptr<Framework::Application> mApplication;

	friend class ConnectorModule;
};
} // namespace Qt
} // namespace Connector
} // namespace Substance
