cmake_minimum_required(VERSION 3.29)
project(original)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(test1 test/test1.cpp original.h)

add_executable(test2 test/test2.cpp original.h)

add_executable(test3 test/test3.cpp original.h)