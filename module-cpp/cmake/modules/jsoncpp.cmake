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

if (TARGET jsoncpp_lib OR TARGET jsoncpp::jsoncpp)
    if (NOT TARGET jsoncpp::jsoncpp AND TARGET jsoncpp_lib)
        add_library(jsoncpp::jsoncpp ALIAS jsoncpp_lib)
        MESSAGE(WARNING "Alias jsoncpp_lib to jsoncpp::jsoncpp")
    endif ()

    return()
endif ()

include(FetchContent)

MESSAGE("Fetching jsoncpp::jsoncpp")

if(NOT CONNECTOR_JSONCPP_URI)
    set(CONNECTOR_JSONCPP_URI "https://github.com/open-source-parsers/jsoncpp.git")
    set(CONNECTOR_JSONCPP_SHA "69098a18b9af0c47549d9a271c054d13ca92b006") # master branch from 08/01/23
endif()

# Set the fetchcontent stuff
FetchContent_Declare(
    "jsoncpp"
    GIT_REPOSITORY ${CONNECTOR_JSONCPP_URI}
    GIT_TAG ${CONNECTOR_JSONCPP_SHA}
)

FetchContent_Populate(
    "jsoncpp"
)

add_subdirectory(
    ${jsoncpp_SOURCE_DIR}
    ${jsoncpp_BINARY_DIR}
    EXCLUDE_FROM_ALL
)

add_library(jsoncpp::jsoncpp ALIAS jsoncpp_static)
