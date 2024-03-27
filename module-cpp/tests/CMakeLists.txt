#[[
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
]]

#########################################################
#                 Connector Framework Tests             #
#########################################################
cmake_minimum_required(VERSION 3.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UNIX_FLAGS "-Wall -Wextra -pedantic -Werror -Wno-error=unused-variable")
set(UNIX_FLAGS "${UNIX_FLAGS} -Wno-error=unused-parameter -pthread")

set(UNIX_FLAGS "-Wall -Wextra -pedantic -Werror -Wno-error=unused-variable")
set(UNIX_FLAGS "${UNIX_FLAGS} -Wno-error=unused-parameter -pthread")

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UNIX_FLAGS} -O2 -mmacosx-version-min=10.11")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")

    add_definitions(-DOSMac_ -DOSMac_Mach0 -D_DARWIN)
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UNIX_FLAGS} -fPIC -O2 -m64")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-W1,-undefined,error -W1,rpath=\'@executable_path\'")
elseif (UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-W1,-undefined,error -shared -Wl,-rpath=\'$ORIGIN\'")
endif()

set(CONNECTOR_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../opt/connector/test/unit_test/")

set(CONNECTOR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../opt/connector/include")

set(CONNECTOR_FRAMEWORK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/framework")

set(CONNECTOR_FRAMEWORK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")

set(CONNECTOR_TEST_COMMON_HEADERS "${CONNECTOR_TEST_INCLUDE_DIR}/common/test_common.h")

set(CONNECTOR_TEST_COMMON_SOURCES "${CONNECTOR_TEST_INCLUDE_DIR}/common/test_common.c")

add_subdirectory("01_test_uuid_compare")
add_subdirectory("02_test_find_callback")
add_subdirectory("03_test_register_export")
add_subdirectory("04_test_init_shutdown")

# Add a custom target to compile all tests
add_custom_target("all_tests"
    DEPENDS test_uuid_compare test_find_callback
            test_register_export test_init_shutdown
)
