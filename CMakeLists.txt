cmake_minimum_required(VERSION 3.29)
project(original)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(math STATIC maths.cpp)

target_include_directories(math PUBLIC maths.h)
