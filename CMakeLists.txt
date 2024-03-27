#[[
Copyright 2023 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
]]

cmake_minimum_required(VERSION 3.22)

project("Substance Connector")

# Build flags to enable various components
option(CONNECTOR_ENABLE_MODULE_CPP "Enable building the C++ framework" OFF)
option(CONNECTOR_ENABLE_MODULE_PYTHON "Enable building the python framework" OFF)
option(CONNECTOR_ENABLE_MODULE_QT "Enable building the Qt C++ framework" OFF)
option(CONNECTOR_ENABLE_CLI "Enable building the CLI tool package for connector" OFF)

if(NOT DEFINED CONNECTOR_ENABLE_CLI 
        AND NOT DEFINED CONNECTOR_ENABLE_MODULE_PYTHON 
        AND NOT DEFINED CONNECTOR_ENABLE_MODULE_CPP 
        AND NOT DEFINED CONNECTOR_ENABLE_MODULE_QT)
    set(CONNECTOR_ENABLE_CLI ON)
    MESSAGE(WARNING "No options specified. Defaulting to building the connector CLI tools")
endif()

# Build the various modules baseed on flags
if (CONNECTOR_ENABLE_CLI)
    add_subdirectory(cli)
else ()
    add_subdirectory(core)
endif ()

if (CONNECTOR_ENABLE_MODULE_CPP)
    add_subdirectory(module-cpp)
endif ()

if (CONNECTOR_ENABLE_MODULE_PYTHON AND NOT DEFINED CONNECTOR_ENABLE_CLI)
    add_subdirectory(module-python)
endif ()

if (CONNECTOR_ENABLE_MODULE_QT)
    if(NOT CONNECTOR_ENABLE_MODULE_CPP)
        add_subdirectory(module-cpp)
    endif()
    add_subdirectory(module-qt)
endif ()