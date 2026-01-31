# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "mbedtls")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(ENABLE_TESTING OFF)
set(ENABLE_PROGRAMS OFF)
set(MBEDTLS_FATAL_WARNINGS OFF)

# Enable installation of mbedtls targets when used as subdirectory
# This is needed when using CPM/FetchContent
set(MBEDTLS_INSTALL ON CACHE BOOL "" FORCE)

if(BUILD_SHARED_LIBS)
    set(USE_SHARED_MBEDTLS_LIBRARY ON)
    set(USE_STATIC_MBEDTLS_LIBRARY OFF)
    add_definitions(-DOCEAN_RUNTIME_SHARED)
else()
    set(USE_SHARED_MBEDTLS_LIBRARY OFF)
    set(USE_STATIC_MBEDTLS_LIBRARY ON)
endif()

find_package(Git REQUIRED)

set(MBEDTLS_GIT_TAG "v3.6.0")

CPMAddPackage(
  NAME           mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
  GIT_TAG        ${MBEDTLS_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/mbedtls.patch"
)

write_library_version("${MBEDTLS_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")