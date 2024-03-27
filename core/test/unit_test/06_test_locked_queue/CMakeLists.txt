set(CONNECTOR_TEST_SOURCES
    test.cpp
)

add_executable(test_locked_queue
    ${CONNECTOR_TEST_SOURCES}
)

target_link_libraries(
    test_locked_queue PRIVATE

    test_common
    connector_details
)

if (UNIX)
    target_link_libraries(
        test_locked_queue PRIVATE
        pthread
    )
endif ()

target_include_directories(
    test_locked_queue PRIVATE

    "${CONNECTOR_TEST_INCLUDE_DIR}"
    "${CONNECTOR_INCLUDE_DIR}"
)

set_target_properties(test_locked_queue PROPERTIES CXX_STANDARD 14)
set_target_properties(test_locked_queue PROPERTIES COMPILE_FLAGS ${SUBSTANCE_CONNECTOR_COMPILE_FLAGS})
