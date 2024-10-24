cmake_minimum_required(VERSION 3.29)
project(original)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(math STATIC maths.cpp)
add_library(array STATIC array.cpp)
add_library(error STATIC error.cpp)

target_include_directories(math PUBLIC maths.h)
target_include_directories(array PUBLIC array.h)
target_include_directories(error PUBLIC error.h)

add_executable(test test.cpp)