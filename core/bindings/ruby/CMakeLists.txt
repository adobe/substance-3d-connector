#########################################################
#                    Substance Connector                #
#########################################################
cmake_minimum_required (VERSION 3.2)

# Determine CMake policies
if (COMMAND cmake_policy)
    # Have the includer be affected by updates
    if (POLICY CMP0011)
        cmake_policy(SET CMP0011 NEW)
    endif(POLICY CMP0011)

    # .vcproj contains relative paths to additional library directories, thus we need to be able to cope with that
    if(POLICY CMP0015)
        cmake_policy(SET CMP0015 NEW)
    endif(POLICY CMP0015)

    # Quoted variables like "MSVC" will no longer be dereferenced
    if(POLICY CMP0054)
        cmake_policy(SET CMP0054 NEW)
    endif(POLICY CMP0054)
endif (COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_C_STANDARD 99)

set(CONNECTOR_PROJECT_NAME "SubstanceConnectorRuby")
project("${CONNECTOR_PROJECT_NAME}")

#########################################################
#                   Project Description                 #
#########################################################

# Acquire the current year for Copyright
string(TIMESTAMP CURRENT_YEAR "%Y" UTC)

# #TODO:: Isolate this to be included somewhere
set(BUILD_MAJOR_VERSION 0)
set(BUILD_MINOR_VERSION 1)
set(BUILD_PATCH_VERSION 3)

set(BUILD_VERSION ${BUILD_MAJOR_VERSION}.${BUILD_MINOR_VERSION}.${BUILD_PATCH_VERSION})

set(PROJECT_NAME        "${CONNECTOR_PROJECT_NAME}")
set(PROJECT_DESCRIPTION "")
set(PROJECT_COPYRIGHT   "Copyright (C) ${CURRENT_YEAR} Allegorithmic")
set(PROJECT_CONTACT     "support@allegorithmic.com")
set(PROJECT_VENDOR      "Allegorithmic")
set(ORG_DOMAIN          "allegorithmic.com")
set(ORG_WEBSITE         "www.allegorithmic.com")

#########################################################
#                       Building                        #
#########################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UNIX_FLAGS "-Wall -Wextra -pedantic -Werror -Wno-error=unused-variable")
set(UNIX_FLAGS "${UNIX_FLAGS} -Wno-error=unused-parameter -Wno-error=unknown-attributes")
set(UNIX_FLAGS "${UNIX_FLAGS} -Wno-error=ignored-attributes")

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UNIX_FLAGS} -O3 -mmacosx-version-min=10.11")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")

    add_definitions(-DOSMac_ -DOSMac_Mach0 -D_DARWIN)
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UNIX_FLAGS} -fPIC -O3 -pthread -m64")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (WIN32)
    # Release initialization
    set(CMAKE_CXX_FLAGS_RELEASE "")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")

    # Warnings as errors
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /WX")

    # Optimization
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /Oy- /GT")

    # Code Generation
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GF /EHa /GS- /Gy /fp:fast")

    # C flags
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /W3 /FC")

    # Debug flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /fp:fast /EHa /FC /MDd")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /fp:fast /EHa /FC /MDd")

    add_definitions(
        -D_WINDOWS
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
        -D_SCL_SECURE_NO_DEPRECATE
        -DISOLATION_AWARE_ENABLED=1
        -D_HYBRID
        -DIS_HYBRID
        -DSECURE_SCL=0
        -DWIN32
        -DWIN64
        -D_USRDLL
        -DNOMINMAX
    )
endif ()

# Set linker flags
if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-W1,-undefined,error -W1,rpath=\'@executable_path\'")
elseif (UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-W1,-undefined,error -shared -Wl,-rpath=\'$ORIGIN\'")
elseif (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /RELEASE")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")

    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SUBSYSTEM:WINDOWS")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the main plugin sources - note that the way that Ruby is currently
# designed means that we will need more complex functionality at even the
# language binding level. Code from the core library can be folded in and
# reused.
set(CONNECTOR_BINDING_SOURCES
    src/connector_ruby.c

    # Add platform independent sources from the core
    ../../src/details/locked_queue.c
)

# Add platform specific sources from the core
if (UNIX)
    set(CONNECTOR_BINDING_SOURCES
        ${CONNECTOR_BINDING_SOURCES}
        ../../src/details/thread_unix.c
    )
elseif (WIN32)
    set(CONNECTOR_BINDING_SOURCES
        ${CONNECTOR_BINDING_SOURCES}
        ../../src/details/thread_windows.c
    )
endif ()

#########################################################
#                       Libraries                       #
#########################################################

find_package(Ruby 2.3 REQUIRED)

find_library(SUBSTANCE_CONNECTOR
             NAMES substance_connector libsubstance_connector
             HINTS "../../build")

add_library(connector SHARED ${CONNECTOR_BINDING_SOURCES})
target_include_directories(
    connector PUBLIC

    "../../include"
    ${RUBY_INCLUDE_DIR}
    ${RUBY_CONFIG_INCLUDE_DIR}
)

# Ruby does not like the "lib" prefix for Unix systems, so remove it
set_target_properties(connector PROPERTIES PREFIX "")

# Ruby expects a bundle file on MacOS, whereas CMake defaults to a dylib
if (APPLE)
    set_target_properties(connector PROPERTIES SUFFIX ".bundle")
endif ()

# Link to both the Substance Connector C module and the Ruby runtime
target_link_libraries(
    connector

    ${SUBSTANCE_CONNECTOR}
    ${RUBY_LIBRARY}
)

# Force visibility to hidden
if (UNIX)
    target_compile_options(
        connector
        PRIVATE
        -fvisibility=hidden
    )
endif ()

#########################################################
#                      Installing                       #
#########################################################
install(
    TARGETS connector
    LIBRARY DESTINATION "."
    RUNTIME DESTINATION "."
)

#########################################################
#                       Packaging                       #
#########################################################
set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_CONTACT}")
set(CPACK_PACKAGE_VERSION_MAJOR "${BUILD_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${BUILD_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${BUILD_PATCH_VERSION}")
set(CPACK_PACKAGE_NAME "SubstanceConnectorRuby")
set(CPACK_PACKAGE_VERSION "${BUILD_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP")

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include (CPack)
