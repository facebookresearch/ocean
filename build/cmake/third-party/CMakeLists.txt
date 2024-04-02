# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

include(${CMAKE_CURRENT_LIST_DIR}/../ocean_utilities.cmake)

project(ocean_third_party)

set(CMAKE_CXX_STANDARD 20)

if (NOT DEFINED OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR)
    option(OCEAN_USE_LONG_CMAKE_INSTALL_LIBDIR "When enabled, additional information like platform and processor names, build type, etc. is used for the installation path of libraries. Otherwise the CMake standard location is used" TRUE)
endif()

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

get_ocean_library_installation_directory(OCEAN_LIBRARY_INSTALL_DIR)
set(CMAKE_INSTALL_LIBDIR "${OCEAN_LIBRARY_INSTALL_DIR}")

message(CHECK_START "Third-party dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

# Do not change the order of these.
include(eigen.cmake)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
