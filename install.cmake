# install.cmake

cmake_minimum_required(VERSION 3.31)

# An option to control whether to clean the build directory after installing, default: ON
# Use -DCLEAN_BUILD_DIR=OFF to turn off, example:
# cmake -P install.cmake -DCLEAN_BUILD_DIR=OFF
option(CLEAN_BUILD_DIR "Remove build directory after install" ON)

get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

set(INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Install directory: ${INSTALL_DIR}")

file(MAKE_DIRECTORY ${INSTALL_DIR}/build)

message(STATUS "Cmake config initializing...")
execute_process(
        COMMAND ${CMAKE_COMMAND} ${PROJECT_ROOT} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
        WORKING_DIRECTORY ${INSTALL_DIR}/build
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake config failed with result ${result}")
endif()

message(STATUS "Building project...")
execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${INSTALL_DIR}/build
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake build failed with result ${result}")
endif()

message(STATUS "Building library files and Header files of project...")
execute_process(
        COMMAND ${CMAKE_COMMAND} --install . --config Release --prefix ${INSTALL_DIR}
        WORKING_DIRECTORY ${INSTALL_DIR}/build
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake install failed with result ${result}")
endif()

message(STATUS "Install success!")
if(CLEAN_BUILD_DIR)
    message(STATUS "Removing temporary build files...")
    file(REMOVE_RECURSE ${INSTALL_DIR}/build)
    message(STATUS "Temporary build files removed...")
endif()
message(STATUS "Library files at: ${INSTALL_DIR}/lib/")
message(STATUS "Header files at: ${INSTALL_DIR}/include/")
