#########################################################
#                      Connector Test                   #
#########################################################
cmake_minimum_required(VERSION 3.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONNECTOR_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CONNECTOR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

set(CONNECTOR_TEST_COMMON_HEADERS "${CONNECTOR_TEST_INCLUDE_DIR}/common/test_common.h")

set(CONNECTOR_TEST_COMMON_SOURCES "${CONNECTOR_TEST_INCLUDE_DIR}/common/test_common.c")

add_library(test_common STATIC ${CONNECTOR_TEST_COMMON_SOURCES} ${CONNECTOR_TEST_COMMON_HEADERS})
target_include_directories(
    test_common PRIVATE
    "."
)

add_library(connector_details STATIC ${CONNECTOR_SOURCES} ${CONNECTOR_HEADERS})
target_include_directories(
    connector_details PRIVATE
    "../../include"
)

set_target_properties(connector_details PROPERTIES COMPILE_FLAGS ${SUBSTANCE_CONNECTOR_COMPILE_FLAGS})
set_target_properties(connector_details PROPERTIES C_STANDARD 90)

target_compile_definitions(
    connector_details
    PRIVATE
    -DSUBSTANCE_CONNECTOR_VERSION="${SUBSTANCE_CONNECTOR_BUILD_VERSION}"
    -DWIN32_LEAN_AND_MEAN=1
)

if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    target_compile_definitions(
        connector_details
        PRIVATE
        -DSUBSTANCE_CONNECTOR_ARCH_64=1
    )
else ()
    target_compile_definitions(
        connector_details
        PRIVATE
        -DSUBSTANCE_CONNECTOR_ARCH_32=1
    )
endif ()

add_subdirectory("01_test_init")
add_subdirectory("02_test_uuid")
add_subdirectory("03_test_allocator")
add_subdirectory("04_test_create_message")
add_subdirectory("05_test_network_utils")
add_subdirectory("06_test_locked_queue")
add_subdirectory("07_test_dispatch")
add_subdirectory("08_test_configuration")
add_subdirectory("09_test_trampoline")
add_subdirectory("10_test_atomic")
add_subdirectory("11_test_uint_queue")
add_subdirectory("12_test_available_queue")
add_subdirectory("13_test_message_queue")
add_subdirectory("14_test_allocator_details")
add_subdirectory("15_test_file_utils")
add_subdirectory("16_test_ensure_directories")
add_subdirectory("17_test_context_state")
add_subdirectory("18_test_read_thread_utilities")
add_subdirectory("19_test_read_thread")
add_subdirectory("20_test_open_tcp")

set(TEST_TARGETS
    test_init
    test_uuid
    test_allocator
    test_create_message
    test_network_utils
    test_locked_queue
    test_dispatch
    test_configuration
    test_trampoline
    test_atomic
    test_uint_queue
    test_available_queue
    test_message_queue
    test_allocator_details
    test_file_utils
    test_ensure_directories
    test_context_state
    test_read_thread_utilities
    test_read_thread
    test_open_tcp
)

add_custom_target("substance_connector_core_tests"
    DEPENDS
    # Libraries shared between testing programs
    test_common
    connector_details

    ${TEST_TARGETS}
)

set_target_properties(
    substance_connector_core_tests
    PROPERTIES
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
    EXCLUDE_FROM_ALL TRUE
)

set_target_properties(
    ${TEST_TARGETS}

    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/substance_connector"
)
