# install.cmake

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(BUILD_DIR "${SOURCE_DIR}/cmake-build-debug/build")
set(INSTALL_DIR "${SOURCE_DIR}/original")

if(NOT DEFINED GENERATOR)
    set(GENERATOR "MinGW Makefiles")
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND}
        -S ${SOURCE_DIR}
        -B ${BUILD_DIR}
        -G ${GENERATOR}
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
        -DBUILD_TESTING=OFF
        RESULT_VARIABLE CONFIGURE_RESULT
)

if(NOT CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake configuration failed.")
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --target install
        RESULT_VARIABLE BUILD_RESULT
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "Build and install failed.")
endif()

message(STATUS "Build and installation completed.")
message(STATUS "Install directory: ${INSTALL_DIR}")