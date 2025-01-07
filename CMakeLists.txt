cmake_minimum_required(VERSION 3.30)
project(original LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/core)

file(GLOB CORE_HEADERS "${CMAKE_SOURCE_DIR}/src/original.h")

add_library(original STATIC ${CORE_HEADERS} src/original.cpp)

install(TARGETS original DESTINATION lib)
install(FILES src/original.h DESTINATION include)
install(DIRECTORY src/core/
        DESTINATION include/core
        FILES_MATCHING PATTERN "*.h")

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/originalConfig.cmake.in"
        "${CMAKE_BINARY_DIR}/cmake/originalConfig.cmake"
        INSTALL_DESTINATION cmake/original
)

install(FILES "${CMAKE_BINARY_DIR}/cmake/originalConfig.cmake"
        DESTINATION cmake/original)

# test cases
add_subdirectory(test/legacy)
add_subdirectory(test/unit_test)
