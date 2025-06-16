# test/unit_test/test_vibrant/CMakeLists.txt

file(GLOB VIBRANT_TESTS "test_*.cpp")

add_executable(vibrant_test ${VIBRANT_TESTS})

target_link_libraries(vibrant_test PRIVATE GTest::gtest GTest::gmock GTest::gmock_main GTest::gtest_main)

target_link_libraries(vibrant_test PRIVATE original)

add_test(NAME VibrantTests COMMAND vibrant_test)
