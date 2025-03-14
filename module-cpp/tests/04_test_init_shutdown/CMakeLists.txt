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

set(TEST_TARGET test_init_shutdown)

set(CONNECTOR_TEST_INIT_SHUTDOWN_SOURCES
    test.cpp
)

add_executable(${TEST_TARGET} ${CONNECTOR_TEST_INIT_SHUTDOWN_SOURCES}
                                   ${CONNECTOR_TEST_COMMON_SOURCES}
                                   ${CONNECTOR_TEST_COMMON_HEADERS})

target_link_libraries(
    ${TEST_TARGET} PRIVATE

    substance_connector
    substanceconnector_framework
)

target_include_directories(
    ${TEST_TARGET} PRIVATE

    "${CONNECTOR_TEST_INCLUDE_DIR}"
    "${CONNECTOR_INCLUDE_DIR}"
    "${CONNECTOR_FRAMEWORK_INCLUDE_DIR}"
)

add_test(NAME "${TEST_TARGET}" COMMAND ${TEST_TARGET})
