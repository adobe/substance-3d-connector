set(TEST_TARGET test_allocator_details)

set(CONNECTOR_TEST_SOURCES
    test.c
)

add_executable(${TEST_TARGET}
    ${CONNECTOR_TEST_SOURCES}
)

target_link_libraries(
    ${TEST_TARGET} PRIVATE

    test_common
    connector_details
)

if (UNIX)
    target_link_libraries(
        ${TEST_TARGET} PRIVATE
        pthread
    )
endif ()

target_include_directories(
    ${TEST_TARGET} PRIVATE

    "${CONNECTOR_TEST_INCLUDE_DIR}"
    "${CONNECTOR_INCLUDE_DIR}"
)

set_target_properties(${TEST_TARGET} PROPERTIES COMPILE_FLAGS ${SUBSTANCE_CONNECTOR_COMPILE_FLAGS})

set_property(TARGET ${TEST_TARGET} PROPERTY C_STANDARD 99)

add_test(NAME "${TEST_TARGET}" COMMAND ${TEST_TARGET})
