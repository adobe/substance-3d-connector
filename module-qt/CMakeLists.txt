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
#                     Connector Qt                     #
#########################################################
set(QT_MAJOR_VERSION 5)
if (Qt6_FOUND)
	set(QT_MAJOR_VERSION 6)
endif()

set(CONNECTOR_QT_SOURCES
    src/application.cpp
    src/export.cpp
    src/module.cpp
    src/system.cpp
)

set(CONNECTOR_QT_HEADERS_MOC "")

if (NOT (DEFINED SUBSTANCE_CONNECTOR_QT_NO_MOC))
    if (Qt5_FOUND)  
        qt5_wrap_cpp(
            CONNECTOR_QT_HEADERS_MOC
            include/substance/connector/qt/application.h
            include/substance/connector/qt/export.h
            include/substance/connector/qt/module.h
            include/substance/connector/qt/system.h
        )
    else()
        qt_wrap_cpp(
            CONNECTOR_QT_HEADERS_MOC
            include/substance/connector/qt/application.h
            include/substance/connector/qt/export.h
            include/substance/connector/qt/module.h
            include/substance/connector/qt/system.h
        )
    endif ()
endif ()

#########################################################
#                       Libraries                       #
#########################################################

if (NOT (DEFINED SUBSTANCE_CONNECTOR_QT_LIBRARY_TARGET_NAME))
    set(SUBSTANCE_CONNECTOR_QT_LIBRARY_TARGET_NAME "substanceconnector_qt")
endif ()

add_library(${SUBSTANCE_CONNECTOR_QT_LIBRARY_TARGET_NAME} STATIC ${CONNECTOR_QT_SOURCES} ${CONNECTOR_QT_HEADERS_MOC})

if (NOT (DEFINED SUBSTANCE_CONNECTOR_FRAMEWORK_LIBRARY_TARGET_NAME))
    set(SUBSTANCE_CONNECTOR_FRAMEWORK_LIBRARY_TARGET_NAME "substanceconnector_framework")
endif ()

if (NOT (DEFINED SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME))
    set(SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME "substance_connector")
endif ()

target_include_directories(
    ${SUBSTANCE_CONNECTOR_QT_LIBRARY_TARGET_NAME}
    PUBLIC

    "include"
)

target_link_libraries(
    ${SUBSTANCE_CONNECTOR_QT_LIBRARY_TARGET_NAME}
    Qt${QT_MAJOR_VERSION}::Core
    ${SUBSTANCE_CONNECTOR_FRAMEWORK_LIBRARY_TARGET_NAME}
    ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
)
