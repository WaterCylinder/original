# test/unit_test/CMakeLists.txt

include(FetchContent)

set(MIRROR_REPO "https://gitee.com/mirrors/googletest.git")
set(GTEST_GIT_REPO "git@github.com:google/googletest.git")
set(GTEST_HTTPS_REPO "https://github.com/google/googletest.git")

set(GTEST_VERSION_STABLE release-1.12.1)
set(GTEST_VERSION_LATEST v1.16.0)

set(REPO_PRIORITY_LIST
        ${GTEST_GIT_REPO}
        ${GTEST_HTTPS_REPO}
        ${MIRROR_REPO}
)

set(INDEX 0)
foreach(repo IN LISTS REPO_PRIORITY_LIST)
    execute_process(
            COMMAND git ls-remote ${repo}
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET
            TIMEOUT 10
    )
    if(result EQUAL 0)
        if (NOT INDEX EQUAL 2)
            set(GTEST_VERSION ${GTEST_VERSION_LATEST})
        else ()
            set(GTEST_VERSION ${GTEST_VERSION_STABLE})
        endif ()

        set(GTEST_REPO ${repo})
        message(STATUS "Using repository: ${repo}")
        break()
    else()
        message(WARNING "Repository unavailable: ${repo}")
    endif()
    math(EXPR INDEX "${INDEX} + 1")
endforeach()

if(NOT GTEST_REPO)
    message(FATAL_ERROR "All repositories are unavailable!")
endif()

FetchContent_Declare(
        GTest
        GIT_REPOSITORY ${GTEST_REPO}
        GIT_TAG        ${GTEST_VERSION}
)

FetchContent_MakeAvailable(GTest)

add_subdirectory(test_core)