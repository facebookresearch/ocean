# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "curl")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CURL_USE_MBEDTLS ON)
set(ENABLE_CURL_MANUAL OFF)
set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)
set(BUILD_CURL_EXE OFF)

if(BUILD_SHARED_LIBS)
    set(BUILD_STATIC_LIBS OFF)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
else()
    set(BUILD_STATIC_LIBS ON)
endif()

if(IOS)
    set(USE_LIBIDN2 OFF)
endif()

set(CURL_GIT_TAG "curl-8_7_1")

CPMAddPackage(
  NAME                curl
  GIT_REPOSITORY      https://github.com/curl/curl.git
  GIT_TAG             ${CURL_GIT_TAG}
)

write_library_version("${CURL_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
