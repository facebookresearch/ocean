# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "curl")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CURL_USE_MBEDTLS ON)
set(ENABLE_CURL_MANUAL OFF)
set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)

CPMAddPackage(
  NAME                curl
  GIT_REPOSITORY      https://github.com/curl/curl.git
  GIT_TAG             curl-8_7_1
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
