# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "googletest")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(BUILD_TESTING OFF)

set(GOOGLETEST_GIT_TAG "v1.14.0")

CPMAddPackage(
  NAME           googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        ${GOOGLETEST_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest.patch"
)

write_library_version("${GOOGLETEST_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
