# install.cmake

cmake_minimum_required(VERSION 3.31)

# An option to control whether to clean the build directory after install, default: ON
# Use -DCLEAN_BUILD_DIR=OFF to turn off, example:
# cmake -P install.cmake -DCLEAN_BUILD_DIR=OFF
option(CLEAN_BUILD_DIR "Remove build directory after install" ON)

get_filename_component(ORIGINAL_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

set(CURRENT_DIR ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Install directory: ${CURRENT_DIR}")

set(ORIGINAL_BUILD_DIR ${CURRENT_DIR}/build)
file(MAKE_DIRECTORY ${ORIGINAL_BUILD_DIR})

message(STATUS "Cmake config initializing...")
execute_process(
        COMMAND ${CMAKE_COMMAND} ${ORIGINAL_ROOT_DIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
        WORKING_DIRECTORY ${ORIGINAL_BUILD_DIR}
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake config failed with result ${result}")
endif()

message(STATUS "Building project...")
execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${ORIGINAL_BUILD_DIR}
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake build failed with result ${result}")
endif()

set(ORIGINAL_INSTALL_DIR ${CURRENT_DIR}/original)

message(STATUS "Installing library files and Header files of project...")
execute_process(
        COMMAND ${CMAKE_COMMAND} --install . --config Release --prefix ${ORIGINAL_INSTALL_DIR}
        WORKING_DIRECTORY ${ORIGINAL_BUILD_DIR}
        RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake install failed with result ${result}")
endif()

message(STATUS "Install success!")
if(CLEAN_BUILD_DIR)
    message(STATUS "Removing temporary build files...")
    file(REMOVE_RECURSE ${ORIGINAL_BUILD_DIR})
    message(STATUS "Temporary build files removed...")
endif()
message(STATUS "Library files at: ${ORIGINAL_INSTALL_DIR}/lib/")
message(STATUS "Header files at: ${ORIGINAL_INSTALL_DIR}/include/")
