# test/other/CMakeLists.txt

enable_testing()

file(GLOB TEST_OTHER "${CMAKE_SOURCE_DIR}/test/other/*.cpp")

foreach(TEST_FILE ${TEST_OTHER})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME} original)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()