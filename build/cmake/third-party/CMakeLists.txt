# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if(CMAKE_SYSTEM_NAME AND "${CMAKE_SYSTEM_NAME}" STREQUAL Android)
  include(${CMAKE_CURRENT_LIST_DIR}/../FindNinjaAndroidSDK.cmake)
endif()

project(ocean_third_party)

include(${CMAKE_CURRENT_LIST_DIR}/../ocean_utilities.cmake)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

message(CHECK_START "Third-party dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/../../third-party_cache")
include(CPM.cmake)

set(CMAKE_PLATFORM_NO_VERSIONED_SONAME TRUE)

set(CMAKE_STAGING_PREFIX ${CMAKE_INSTALL_PREFIX})

include(${INCLUDED_DEP_NAME}.cmake)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
