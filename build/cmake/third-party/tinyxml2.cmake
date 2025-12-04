# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "tinyxml2")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(BUILD_TESTING OFF)

set(TINYXML2_GIT_TAG "10.0.0")

CPMAddPackage(
  NAME           tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG        ${TINYXML2_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/tinyxml2.patch"
)

write_library_version("${TINYXML2_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
