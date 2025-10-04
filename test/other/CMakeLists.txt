# test/other/CMakeLists.txt

enable_testing()

file(GLOB TEST_OTHER "${CMAKE_SOURCE_DIR}/test/other/*.cpp")

foreach(TEST_FILE ${TEST_OTHER})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME} original)

    add_test(
            NAME other_${TEST_NAME}
            COMMAND $<TARGET_FILE:${TEST_NAME}>
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()