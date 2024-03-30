# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

project(ocean)

include(${CMAKE_CURRENT_LIST_DIR}/build/cmake/ocean_utilities.cmake)

# Build options
option(BUILD_SHARED_LIBS "Build using shared libraries" FALSE)
option(OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR "When enabled, additional information like platform and processor names, build type, etc. is used for the installation path of libraries. Otherwise the CMake standard location is used" TRUE)
option(OCEAN_ENABLE_MESSENGER "When enabled, the logging mechanism in Ocean will be enabled. Otherwise it will be disabled" TRUE)

# Build config and definitions
set(CMAKE_CXX_STANDARD 20)

set(OCEAN_IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/impl")

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

if (OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR)
    if (BUILD_SHARED_LIBS)
        set(OCEAN_LINKING_TYPE_STRING "shared")
    else()
        set(OCEAN_LINKING_TYPE_STRING "static")
    endif()

    string(TOLOWER "lib/${OCEAN_TARGET_SYSTEM_NAME}_${OCEAN_TARGET_PROCESSOR_TYPE}_${OCEAN_LINKING_TYPE_STRING}_${CMAKE_BUILD_TYPE}" OCEAN_LIBRARY_INSTALL_DIR)
else()
    set(OCEAN_LIBRARY_INSTALL_DIR "lib")
endif()

get_ocean_preprocessor_flags(OCEAN_PREPROCESSOR_FLAGS)
get_ocean_compiler_flags(OCEAN_COMPILER_FLAGS)

add_subdirectory(impl/application/ocean/demo/base/console)
add_subdirectory(impl/application/ocean/test/base/testbase)
add_subdirectory(impl/ocean/base)
add_subdirectory(impl/ocean/system)
add_subdirectory(impl/ocean/test/testbase)

message(STATUS " ")
message(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OCEAN_COMPILER_FLAGS = \"${OCEAN_COMPILER_FLAGS}\"")
message(STATUS "OCEAN_ENABLE_MESSENGER = ${OCEAN_ENABLE_MESSENGER}")
message(STATUS "OCEAN_HOST_PROCESSOR_TYPE = ${OCEAN_HOST_PROCESSOR_TYPE}")
message(STATUS "OCEAN_HOST_SYSTEM_NAME = ${OCEAN_HOST_SYSTEM_NAME}")
message(STATUS "OCEAN_IMPL_DIR = ${OCEAN_IMPL_DIR}")
message(STATUS "OCEAN_LIBRARY_INSTALL_DIR = ${OCEAN_LIBRARY_INSTALL_DIR}")
message(STATUS "OCEAN_PREPROCESSOR_FLAGS = \"${OCEAN_PREPROCESSOR_FLAGS}\"")
message(STATUS "OCEAN_TARGET_PROCESSOR_TYPE = ${OCEAN_TARGET_PROCESSOR_TYPE}")
message(STATUS "OCEAN_TARGET_SYSTEM_NAME = ${OCEAN_TARGET_SYSTEM_NAME}")
message(STATUS "OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR = ${OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR}")
message(STATUS " ")
