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

//! @file system.h
//! @brief QObject implementation of the system interface
//! @author Galen Helfter - Adobe
//! @date 20190815
//! @copyright Adobe. All rights reserved.

#pragma once

#include <substance/connector/qt/application.h>

namespace Substance
{
namespace Connector
{
namespace Qt
{
class ConnectorSystem : public ConnectorApplication
{
	Q_OBJECT

public:
	ConnectorSystem(QObject* parent = nullptr);

	virtual ~ConnectorSystem();

	//! @brief Set up the internal state for this implementation
	void initialize() override;

	//! @brief Tear down the internal state
	void shutdown() override;

Q_SIGNALS:
	//! @brief Signal that is emitted when a connection recevied message is received
	//! @param context Integer representing the underlying connection
	//! @param message String value of the received message
	void recvConnectionEstablished(unsigned int context, const QString& message);

	//! @brief Signal that is emitted when a connection closed message is received
	//! @param context Integer representing the underlying connection
	//! @param message String value of the received message
	void recvConnectionClosed(unsigned int context, const QString& message);

	//! @brief Signal that is emitted when a endpoint is sending the context / feature level
	//! @param context Integer representing the underlying connection
	//! @param message String value of the received message
	void updateConnectionContext(unsigned int context, const QString& message);
};
} // namespace Qt
} // namespace Connector
} // namespace Substance

