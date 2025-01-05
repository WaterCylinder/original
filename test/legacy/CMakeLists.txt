# test/legacy/CMakeLists.txt

add_executable(test1 test1.cpp)
add_executable(test2 test2.cpp)
add_executable(test3 test3.cpp)
add_executable(test4 test4.cpp)

target_link_libraries(test1 original)
target_link_libraries(test2 original)
target_link_libraries(test3 original)
target_link_libraries(test4 original)

target_include_directories(test1 PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_include_directories(test2 PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_include_directories(test3 PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_include_directories(test4 PRIVATE ${CMAKE_SOURCE_DIR}/src/core)