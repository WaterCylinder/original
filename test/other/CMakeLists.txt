# test/other/CMakeLists.txt

file(GLOB TEST_LEGACY "${CMAKE_SOURCE_DIR}/test/other/*.cpp")

foreach(TEST_FILE ${TEST_LEGACY})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} original)
endforeach()