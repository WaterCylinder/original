# install.cmake

cmake_minimum_required(VERSION 3.31)

# An option to control whether to clean the build directory after install, default: ON
if(DEFINED ENV{CLEAN_BUILD_DIR})
    if("$ENV{CLEAN_BUILD_DIR}" MATCHES "^(ON|TRUE|YES|1|true|yes|on)$")
        set(CLEAN_BUILD_DIR TRUE)
    else()
        set(CLEAN_BUILD_DIR FALSE)
    endif()
else()
    set(CLEAN_BUILD_DIR TRUE)
endif()

# An option allowing user to specify installation prefix
if(DEFINED ENV{INSTALL_PREFIX})
    set(INSTALL_PREFIX "$ENV{INSTALL_PREFIX}")
else()
    set(INSTALL_PREFIX "")
endif()

# An option allowing user to specify build system generator to use (e.g. Ninja, Unix Makefiles)
if(DEFINED ENV{GENERATOR})
    set(GENERATOR "$ENV{GENERATOR}")
else()
    set(GENERATOR "")
endif()

# Option usage example:
# 1. On Windows PowerShell:
#    $env:GENERATOR="Ninja"; $env:INSTALL_PREFIX="D:\your\path"; $env:CLEAN_BUILD_DIR="false"; cmake -P install.cmake
# 2. On Windows Command Prompt (CMD):
#    set GENERATOR=Ninja && set INSTALL_PREFIX=D:\your\path && set CLEAN_BUILD_DIR=false && cmake -P install.cmake
# 3. On Linux/macOS (Bash):
#    GENERATOR="Ninja" INSTALL_PREFIX="/your/path" CLEAN_BUILD_DIR="false" cmake -P install.cmake

# -- DEBUG -- #
set(ENABLE_DEBUG FALSE)
if (ENABLE_DEBUG)
    message(WARNING "[DEBUG]: GENERATOR: ${GENERATOR}")
    message(WARNING "[DEBUG]: INSTALL_PREFIX: ${INSTALL_PREFIX}")
    if (CLEAN_BUILD_DIR)
        message(WARNING "[DEBUG]: CLEAN_BUILD_DIR: true")
    else ()
        message(WARNING "[DEBUG]: CLEAN_BUILD_DIR: false")
    endif ()
endif ()

message(STATUS "Cmake config initializing...")
get_filename_component(ORIGINAL_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

set(CURRENT_DIR ${CMAKE_CURRENT_BINARY_DIR})
if(INSTALL_PREFIX STREQUAL "")
    message(STATUS "No install prefix specified, using default: ${CURRENT_DIR}")
else()
    message(STATUS "Use specified install directory: ${INSTALL_PREFIX}")
endif()

set(ORIGINAL_BUILD_DIR ${CURRENT_DIR}/build)
file(MAKE_DIRECTORY ${ORIGINAL_BUILD_DIR})

if(WIN32)
    set(DEFAULT_GENERATOR "MinGW Makefiles")
else()
    set(DEFAULT_GENERATOR "Unix Makefiles")
endif()
if(NOT GENERATOR STREQUAL "")
    set(TRY_GENERATOR "${GENERATOR}")
    message(STATUS "Use specified generator: ${TRY_GENERATOR}")
else()
    set(TRY_GENERATOR "${DEFAULT_GENERATOR}")
    message(STATUS "No generator specified, using default: ${TRY_GENERATOR}")
endif()

message(STATUS "Trying generator: ${TRY_GENERATOR}")
execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${TRY_GENERATOR}" ${ORIGINAL_ROOT_DIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
        WORKING_DIRECTORY ${ORIGINAL_BUILD_DIR}
        RESULT_VARIABLE check_result
        OUTPUT_QUIET
        ERROR_QUIET
)
if(NOT check_result EQUAL 0)
    if(NOT GENERATOR STREQUAL "")
        message(WARNING "User-specified generator \"${TRY_GENERATOR}\" not found.")
        file(REMOVE_RECURSE ${ORIGINAL_BUILD_DIR})
        file(MAKE_DIRECTORY ${ORIGINAL_BUILD_DIR})
        set(TRY_GENERATOR "${DEFAULT_GENERATOR}")
        message(WARNING "Try to use default: ${TRY_GENERATOR}")
    else()
        message(FATAL_ERROR "Default generator \"${TRY_GENERATOR}\" not found. Please install it and retry.")
    endif()
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${TRY_GENERATOR}" ${ORIGINAL_ROOT_DIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
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

if(INSTALL_PREFIX STREQUAL "")
    set(ORIGINAL_INSTALL_DIR ${CURRENT_DIR}/original)
else()
    get_filename_component(_last_dir ${INSTALL_PREFIX} NAME)
    if(_last_dir STREQUAL "original")
        set(ORIGINAL_INSTALL_DIR ${INSTALL_PREFIX})
    else()
        set(ORIGINAL_INSTALL_DIR ${INSTALL_PREFIX}/original)
    endif()
endif()

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
else ()
    message(STATUS "Build files at: ${ORIGINAL_BUILD_DIR}")
endif()
message(STATUS "Library files at: ${ORIGINAL_INSTALL_DIR}/lib/")
message(STATUS "Header files at: ${ORIGINAL_INSTALL_DIR}/include/")
