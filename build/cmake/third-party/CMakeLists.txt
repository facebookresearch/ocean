# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

include(${CMAKE_CURRENT_LIST_DIR}/../ocean_utilities.cmake)

project(ocean_third_party)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

message(CHECK_START "Third-party dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

# Do not change the order of these.
if(BUILD_PASS_INDEX EQUAL 0)
    include(libjpeg.cmake)
    include(gl.cmake)
    include(zlib.cmake)
    include(eigen.cmake)
    include(giflib.cmake)
elseif(BUILD_PASS_INDEX EQUAL 1)
    include(libpng.cmake)
    include(assimp.cmake)
    include(freetype.cmake)
endif()

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
