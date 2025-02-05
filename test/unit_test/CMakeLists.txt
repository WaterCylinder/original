# test/unit_test/CMakeLists.txt

include(FetchContent)

FetchContent_Declare(
        GTest
        GIT_REPOSITORY git@github.com:/google/googletest.git
        GIT_TAG release-1.11.0
)

FetchContent_MakeAvailable(GTest)

add_subdirectory(test_core)