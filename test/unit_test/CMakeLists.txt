# test/unit_test/CMakeLists.txt

include(FetchContent)

set(MIRROR_REPO "https://gitee.com/mirrors/googletest.git")
set(GTEST_GIT_REPO "git@github.com:google/googletest.git")
set(GTEST_HTTPS_REPO "https://github.com/google/googletest.git")

set(REPO_PRIORITY_LIST
        ${GTEST_GIT_REPO}
        ${GTEST_HTTPS_REPO}
        ${MIRROR_REPO}
)

foreach(repo IN LISTS REPO_PRIORITY_LIST)
    execute_process(
            COMMAND git ls-remote ${repo}
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET
            TIMEOUT 10
    )
    if(result EQUAL 0)
        set(GTEST_REPO ${repo})
        message(STATUS "Using repository: ${repo}")
        break()
    else()
        message(WARNING "Repository unavailable: ${repo}")
    endif()
endforeach()

if(NOT GTEST_REPO)
    message(FATAL_ERROR "All repositories are unavailable!")
endif()

FetchContent_Declare(
        GTest
        GIT_REPOSITORY ${GTEST_REPO}
        GIT_TAG        v1.16.0
)

FetchContent_MakeAvailable(GTest)

add_subdirectory(test_core)