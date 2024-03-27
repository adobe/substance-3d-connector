#########################################################
#                    Substance Connector                #
#########################################################
cmake_minimum_required (VERSION 3.2)

#########################################################
#                   Project Description                 #
#########################################################

set(BUILD_MAJOR_VERSION 0)
set(BUILD_MINOR_VERSION 3)
set(BUILD_PATCH_VERSION 0)

set(BUILD_VERSION ${BUILD_MAJOR_VERSION}.${BUILD_MINOR_VERSION}.${BUILD_PATCH_VERSION})

set(CONNECTOR_BINDING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/broadcast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/connection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/initshutdown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/methodtable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/trampoline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/uuid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/version.c
)

set(CONNECTOR_BINDING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/broadcast.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/initshutdown.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/methodtable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/trampoline.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/uuid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/python/details/version.h
)

if (NOT (DEFINED ${CONNECTOR_PYTHON_TARGET_NAME}))
    set(CONNECTOR_PYTHON_TARGET_NAME "connector-modules-python")
endif ()

#########################################################
#                       Libraries                       #
#########################################################

# CMake is going to default to Python 3, so we'll need the option to
# change

if (DEFINED SUBSTANCE_CONNECTOR_CUSTOM_PYTHON_TARGET)
    set(CONNECTOR_PYTHON_LIB ${SUBSTANCE_CONNECTOR_CUSTOM_PYTHON_TARGET})
elseif (DEFINED SUBSTANCE_CONNECTOR_PYTHON_2)
    find_package(Python2 REQUIRED COMPONENTS Development)
    set(CONNECTOR_PYTHON_LIB Python2::Python)
elseif (DEFINED SUBSTANCE_CONNECTOR_PYTHON_3)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    set(CONNECTOR_PYTHON_LIB Python3::Module)
else ()
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    set(CONNECTOR_PYTHON_LIB Python::Module)
endif()

add_library(${CONNECTOR_PYTHON_TARGET_NAME} SHARED ${CONNECTOR_BINDING_SOURCES})
target_include_directories(
    ${CONNECTOR_PYTHON_TARGET_NAME}
 
    PRIVATE
    "include"
    "lib/core/include"
)

set_target_properties(${CONNECTOR_PYTHON_TARGET_NAME} PROPERTIES OUTPUT_NAME "connector")

# Python doesn't want a "lib" prefix
set_target_properties(${CONNECTOR_PYTHON_TARGET_NAME} PROPERTIES PREFIX "")

# Python expects a .so file on MacOS, whereas CMake defaults to a dylib
if (APPLE)
    set_target_properties(${CONNECTOR_PYTHON_TARGET_NAME} PROPERTIES SUFFIX ".so")
elseif (MSVC)
    set_target_properties(${CONNECTOR_PYTHON_TARGET_NAME} PROPERTIES SUFFIX ".pyd")
endif ()

# Connector to both the Substance connector C module and the Ruby runtime
target_link_libraries(
    ${CONNECTOR_PYTHON_TARGET_NAME}

    PRIVATE
    substance_connector
    ${CONNECTOR_PYTHON_LIB}
)

# Set runtime path for unix systems
if (APPLE)
    target_compile_options(
        ${CONNECTOR_PYTHON_TARGET_NAME}
        PRIVATE
        "-W1,-undefined,error -W1,rpath \'@executable_path\'"
    )

    add_custom_command(
        TARGET ${CONNECTOR_PYTHON_TARGET_NAME}
        POST_BUILD
        COMMAND
        "install_name_tool"
        "-change"
        "@rpath/libsubstance_connector.dylib"
        "@loader_path/libsubstance_connector.dylib"
        $<TARGET_FILE:${CONNECTOR_PYTHON_TARGET_NAME}>
    )
elseif (UNIX)
    target_compile_options(
        ${CONNECTOR_PYTHON_TARGET_NAME}
        PRIVATE
        "-Wl,-undefined,error -fvisibilty=hidden -shared"
    )
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/'$ORIGIN/'") 
endif ()

# Force visibility to hidden
if (UNIX)
    target_compile_options(
        ${CONNECTOR_PYTHON_TARGET_NAME}
        PRIVATE
        -fvisibility=hidden
    )
endif ()

add_subdirectory(lib)

# If the user has set an install location, install everything to
# that folder in the proper layout
if (DEFINED CONNECTOR_PYTHON_INSTALL_LOCATION)
    install(
        TARGETS
        ${CONNECTOR_PYTHON_TARGET_NAME}

        DESTINATION
        ${CONNECTOR_PYTHON_INSTALL_LOCATION}/substanceconnector/core
    )
endif ()
