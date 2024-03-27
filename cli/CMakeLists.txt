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

cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CONNECTOR_PYTHON_INSTALL_LOCATION)
    set(CONNECTOR_PYTHON_INSTALL_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/install")
endif()

install(
    FILES
    "main.py"
    "requirements.txt"
    DESTINATION
    ${CONNECTOR_PYTHON_INSTALL_LOCATION}
)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../core
  ${CMAKE_CURRENT_BINARY_DIR}/core
)
add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../module-python
  ${CMAKE_CURRENT_BINARY_DIR}/module-python
)

install(
    TARGETS
    substance_connector
    DESTINATION
    ${CONNECTOR_PYTHON_INSTALL_LOCATION}/substanceconnector/core
)
