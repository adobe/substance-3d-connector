#########################################################
#                    Substance Connector                #
#########################################################
cmake_minimum_required(VERSION 3.2)

set(SUBSTANCE_CONNECTOR_MAJOR_VERSION 1 CACHE INTERNAL "Version")
set(SUBSTANCE_CONNECTOR_MINOR_VERSION 0 CACHE INTERNAL "Version")
set(SUBSTANCE_CONNECTOR_PATCH_VERSION 0 CACHE INTERNAL "Version")

set(SUBSTANCE_CONNECTOR_VERSION ${SUBSTANCE_CONNECTOR_MAJOR_VERSION}.${SUBSTANCE_CONNECTOR_MINOR_VERSION}.${SUBSTANCE_CONNECTOR_PATCH_VERSION} CACHE INTERNAL "Version")
set(SUBSTANCE_CONNECTOR_CURRENT_YEAR 2025 CACHE INTERNAL "Version")

configure_file(src/version.rc.in src/version.rc)

# Set the main plugin sources
set(CONNECTOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external_api.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/available_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/communication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/configuration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/connection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/connection_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/context_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/disconnect_message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/dispatch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/internal_messages.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/internal_uuids.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/locked_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/message_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/state.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/string_map.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/string_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/uint_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/uuid_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/network/autoconnect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/network/openconnectionimpl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/network/readwriteutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/system/connectiondirectory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/system/fileutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/system/pathutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/threadimpl/read_threads.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/threadimpl/readstructimpl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/threadimpl/readthreadimpl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/details/threadimpl/write_threads.c
)

# Handle platform-specific source files
if (UNIX)
    set(CONNECTOR_SOURCES
        ${CONNECTOR_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/details/connection_unix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/details/thread_unix.c
    )
elseif (WIN32)
    set(CONNECTOR_SOURCES
        ${CONNECTOR_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/details/connection_windows.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/details/thread_windows.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/version.rc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
    )
endif ()

set(CONNECTOR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/errorcodes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/connector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/types.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/atomic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/available_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/callbacks.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/communication.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/configuration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/connection_details.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/connection_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/context_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/context_struct.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/disconnect_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/dispatch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/internal_messages.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/internal_uuids.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/locked_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/message_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/state.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/string_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/string_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/uint_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/uuid_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/network/autoconnect.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/network/openconnectionimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/network/readwriteutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/system/connectiondirectory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/system/fileutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/system/pathstringdetails.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/system/pathtype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/system/pathutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/threadimpl/read_threads.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/threadimpl/readstructimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/threadimpl/readthreadimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/substance/connector/details/threadimpl/write_threads.h
)

#########################################################
#                       Libraries                       #
#########################################################

if (NOT (DEFINED SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME))
    set(SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME "substance_connector")
endif ()

if (DEFINED SUBSTANCE_CONNECTOR_STATIC)
    add_library(${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} STATIC ${CONNECTOR_SOURCES} ${CONNECTOR_HEADERS})
else ()
    add_library(${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} SHARED ${CONNECTOR_SOURCES} ${CONNECTOR_HEADERS})
endif ()

set_target_properties(${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} PROPERTIES OUTPUT_NAME "substance_connector")

target_include_directories(
    ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} PUBLIC

    "include"
)

if (UNIX)
    target_compile_options(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        PRIVATE
        -fvisibility=hidden
    )
    target_link_libraries(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        INTERFACE
        "-fvisibility=hidden"
    )
endif ()

if (WIN32)
    target_link_libraries(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        wsock32
        ws2_32
    )
endif ()

string(TIMESTAMP CURRENT_YEAR "%Y" UTC)

target_compile_definitions(
    ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
    PRIVATE
    -DSUBSTANCE_CONNECTOR_VERSION="${SUBSTANCE_CONNECTOR_VERSION}"
    -DSUBSTANCE_CONNECTOR_MAJOR_VERSION=${SUBSTANCE_CONNECTOR_MAJOR_VERSION}
    -DSUBSTANCE_CONNECTOR_MINOR_VERSION=${SUBSTANCE_CONNECTOR_MINOR_VERSION}
    -DSUBSTANCE_CONNECTOR_PATCH_VERSION=${SUBSTANCE_CONNECTOR_PATCH_VERSION}
    -DSUBSTANCE_CONNECTOR_CURRENT_YEAR="${CURRENT_YEAR}"
    # Force at least Windows Vista
    -D_WIN32_WINNT=0x600
)

set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "")

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -Wno-error=unused-but-set-variable")
endif ()

if (UNIX)
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -Wall -Wextra")
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -pedantic -Wno-error=pedantic")
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -fvisibility=hidden")
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -Wno-error=unused-parameter -Wno-error=unused-variable")
    if (APPLE)
        set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -mmacosx-version-min=10.11")
    else ()
        set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} -fPIC -pthread")
    endif ()
elseif (WIN32)
    set(SUBSTANCE_CONNECTOR_COMPILE_FLAGS "${SUBSTANCE_CONNECTOR_COMPILE_FLAGS} /wd4800 /MD")
    target_compile_definitions(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        PRIVATE
        -DWIN32_LEAN_AND_MEAN=1
    )
endif ()

set_target_properties(${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} PROPERTIES COMPILE_FLAGS ${SUBSTANCE_CONNECTOR_COMPILE_FLAGS})

set_target_properties(${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME} PROPERTIES C_STANDARD 90)

if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    target_compile_definitions(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        PRIVATE
        -DSUBSTANCE_CONNECTOR_ARCH_64=1
    )
else ()
    target_compile_definitions(
        ${SUBSTANCE_CONNECTOR_LIBRARY_TARGET_NAME}
        PRIVATE
        -DSUBSTANCE_CONNECTOR_ARCH_32=1
    )
endif ()

if (CONNECTOR_ENABLE_BUILD_TESTS)
    add_subdirectory("test/unit_test")
endif ()
