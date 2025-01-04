cmake_minimum_required(VERSION 3.29)
project(original)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(test1 test/legacy/test1.cpp src/original.h)

add_executable(test2 test/legacy/test2.cpp src/original.h)

add_executable(test3 test/legacy/test3.cpp src/original.h)

add_executable(test4 test/legacy/test4.cpp src/original.h)