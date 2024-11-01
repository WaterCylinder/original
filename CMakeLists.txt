cmake_minimum_required(VERSION 3.29)
project(original)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(test test.cpp vector.h)