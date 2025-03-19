# test/unit_test/test_core/CMakeLists.txt

file(GLOB CORE_TESTS "test_*.cpp")

add_executable(core_tests ${CORE_TESTS})

target_link_libraries(core_tests PRIVATE GTest::gtest GTest::gmock GTest::gmock_main GTest::gtest_main)

target_link_libraries(core_tests PRIVATE original)

add_test(NAME CoreTests COMMAND core_tests)
