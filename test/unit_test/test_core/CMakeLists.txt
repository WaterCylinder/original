# test/unit_test/test_core/CMakeLists.txt

file(GLOB CORE_TESTS "test_*.cpp")

add_executable(core_tests ${CORE_TESTS})

target_link_libraries(core_tests original)
target_link_libraries(core_tests GTest::gtest GTest::gmock GTest::gmock_main GTest::gtest_main)

add_test(NAME CoreTests COMMAND core_tests)
