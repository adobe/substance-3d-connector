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

//! @file core.cpp
//! @brief Defines core operations for Substance Connector
//! @author Galen Helfter - Adobe
//! @date 20190411
//! @copyright Adobe. All rights reserved.

#include <substance/connector/types.h>
#include <substance/connector/connector.h>
#include <substance/connector/errorcodes.h>

#include <string>
#include <set>
#include <atomic>

#include <substance/connector/framework/core.h>
#include <substance/connector/framework/application.h>
#include <substance/connector/framework/details/callbacks.h>
#include <substance/connector/framework/details/dynamicload.h>
#include <substance/connector/framework/details/connectionschema.h>

extern "C"
{
	static void connector_cpp_trampoline(unsigned int context,
										 const substance_connector_uuid_t* messageType,
										 const char* message)
	{
		// Acquire the callback associated with the message
		auto callbacks = Substance::Connector::Framework::Details::findCallbacks(*messageType);
		if (callbacks.size() > 0)
		{
			for (const auto& callbackItr : callbacks)
			{
				if (callbackItr != nullptr)
				{
					callbackItr(context, messageType, message);
				}
			}
		}
	}
}

namespace Substance
{
namespace Connector
{
namespace Framework
{
enum class StartupState : size_t
{
	CONNECTOR_SHUTDOWN = 0x00u,
	CONNECTOR_INIT_STARTED = 0x01u,
	CONNECTOR_INITIALIZED = 0x02u,
	CONNECTOR_SHUTDOWN_STARTED = 0x03u,
	CONNECTOR_ADDING_APPLICATION = 0x04u
};

static std::set<Application*> applications;
static std::atomic<StartupState> startupState;
static std::vector<std::string> featureLevel;
static std::string connectionDisplayName;
static std::string connectionId;

static void preInit()
{
	for (auto& application : applications)
	{
		application->preInit();
	}
}

static void postInit()
{
	for (auto& application : applications)
	{
		application->postInit();
	}
}

static void preShutdown()
{
	for (auto& application : applications)
	{
		application->preShutdown();
	}
}

static void postShutdown()
{
	for (auto& application : applications)
	{
		application->postShutdown();
	}
}

const char* version()
{
	return CONNECTOR_FRAMEWORK_CALL(version)();
}

bool registerApplication(Application* application)
{
	bool result = false;

	StartupState expected = StartupState::CONNECTOR_SHUTDOWN;
	bool swapped = startupState.compare_exchange_strong(expected, StartupState::CONNECTOR_ADDING_APPLICATION);

	if (swapped)
	{
		if (application != nullptr && (applications.find(application) == applications.end()))
		{
			applications.insert(application);
			result = true;
			const auto& featureIds = application->getFeatureIds();
			for(const auto& id : featureIds)
			{
				// NOTE:: This is a good candidate for a util function
				std::string uuidString;
				for (size_t i = 0u; i < 4u; ++i)
				{
					if(uuidString.size() > 0)
					{
						uuidString += '-';
					}
          std::stringstream ss;
          ss << std::hex << id.elements[i];
					uuidString += std::string(ss.str());
				}
				featureLevel.push_back(uuidString);
			}
		}

		// Revert back to a shutdown state after successfully registering the
		// application with the underlying system.
		startupState.store(StartupState::CONNECTOR_SHUTDOWN);
	}

	return result;
}

bool init(const std::string& display_name, void* module)
{
  return init(display_name, display_name, module);
}

bool init(const std::string& display_name, const std::string& id, void* module)
{
    connectionDisplayName = display_name;
    connectionId = id;

	bool result = false;

	StartupState expected = StartupState::CONNECTOR_SHUTDOWN;
	bool swapped = startupState.compare_exchange_strong(expected, StartupState::CONNECTOR_INIT_STARTED);

	if (swapped)
	{
		Details::initDynamicLoad(module);

		preInit();

		result = (CONNECTOR_FRAMEWORK_CALL(init)(connectionId.c_str()) == SUBSTANCE_CONNECTOR_SUCCESS);

		if (result)
		{
			CONNECTOR_FRAMEWORK_CALL(add_trampoline)(connector_cpp_trampoline);

			postInit();

			startupState.store(StartupState::CONNECTOR_INITIALIZED);
		}
	}

	return result;
}

std::string getConnectionContext() {
		Details::connection_schema connection_details;
		connection_details.display_name = connectionDisplayName;
		connection_details.id_name = connectionId;
		connection_details.connector_version = version();
		connection_details.available_features = featureLevel;
		std::string connectionJson = connection_details.GetJsonString();
    return std::move(connectionJson);
}

bool shutdown()
{
	bool result = false;

	StartupState expected = StartupState::CONNECTOR_INITIALIZED;
	bool swapped = startupState.compare_exchange_strong(expected, StartupState::CONNECTOR_SHUTDOWN_STARTED);

	if (swapped)
	{
		preShutdown();

		result = (CONNECTOR_FRAMEWORK_CALL(shutdown)() == SUBSTANCE_CONNECTOR_SUCCESS);

		if (result)
		{
			postShutdown();

			startupState.store(StartupState::CONNECTOR_SHUTDOWN);

			applications.clear();
			featureLevel.clear();
		}
	}

	return result;
}

bool openTcpConnection(unsigned int port, unsigned int* context)
{
	return (CONNECTOR_FRAMEWORK_CALL(open_tcp)(port, context) == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool establishTcpConnection(unsigned int port, unsigned int* context)
{
	return (CONNECTOR_FRAMEWORK_CALL(connect_tcp)(port, context) == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool openDefaultTcp(unsigned int* context)
{
	return (CONNECTOR_FRAMEWORK_CALL(open_default_tcp)(context) == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool broadcastTcp()
{
	return (CONNECTOR_FRAMEWORK_CALL(broadcast_tcp)() == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool openDefaultUnix(unsigned int* context)
{
	return (CONNECTOR_FRAMEWORK_CALL(open_default_unix)(context) == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool broadcastUnix()
{
	return (CONNECTOR_FRAMEWORK_CALL(broadcast_unix)() == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool openDefault(unsigned int* context)
{
	return (CONNECTOR_FRAMEWORK_CALL(open_default)(context) == SUBSTANCE_CONNECTOR_SUCCESS);
}

bool broadcastDefault()
{
	return (CONNECTOR_FRAMEWORK_CALL(broadcast_default)() == SUBSTANCE_CONNECTOR_SUCCESS);
}
} // namespace Framework
} // namespace Connector
} // namespace Substance
