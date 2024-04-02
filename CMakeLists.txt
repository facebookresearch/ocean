# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

# Make option() honor normal variables
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # For CMake subprojects

project(ocean)

include(${CMAKE_CURRENT_LIST_DIR}/build/cmake/ocean_utilities.cmake)

# Build options
option(BUILD_SHARED_LIBS "Build using shared libraries" FALSE)
option(OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR "When enabled, additional information like platform and processor names, build type, etc. is used for the installation path of libraries. Otherwise the CMake standard location is used" TRUE)
option(OCEAN_ENABLE_MESSENGER "When enabled, the logging mechanism in Ocean will be enabled. Otherwise it will be disabled" TRUE)
option(OCEAN_BUILD_DEMOS "When enabled, the desktop demo applications will be included in the build. Otherwise they will be ignored." TRUE)
option(OCEAN_BUILD_TESTS "When enabled, the desktop test libraries and applications will be included in the build. Otherwise they will be ignored." TRUE)
option(OCEAN_BUILD_THIRD_PARTY_LIBS "When enabled, the third-party libraries required by Ocean will be built together with Ocean itself. Otherwise they will be searched using the usual CMake search capabilities" FALSE)

# Build config and definitions
set(CMAKE_CXX_STANDARD 20)

set(OCEAN_IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/impl")

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

get_ocean_library_installation_directory(OCEAN_LIBRARY_INSTALL_DIR)

get_ocean_preprocessor_flags(OCEAN_PREPROCESSOR_FLAGS)
get_ocean_compiler_flags(OCEAN_COMPILER_FLAGS)

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

# Third-party libraries required by Ocean
if (OCEAN_BUILD_THIRD_PARTY_LIBS)
    add_subdirectory(build/cmake/third-party)
endif()

# Ocean libraries
add_subdirectory(impl/ocean/base)
add_subdirectory(impl/ocean/math)
add_subdirectory(impl/ocean/system)

# Demo applications (for desktop)
if (OCEAN_BUILD_DEMOS)
    add_subdirectory(impl/application/ocean/demo/base/console)
endif()

if (OCEAN_BUILD_TESTS)
    # Test libraries
    add_subdirectory(impl/ocean/test/testbase)
    add_subdirectory(impl/ocean/test/testmath)

    # Test applications
    add_subdirectory(impl/application/ocean/test/base/testbase)
    add_subdirectory(impl/application/ocean/test/math/testmath)
endif()
