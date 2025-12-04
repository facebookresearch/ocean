# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "protozero")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(BUILD_TESTING OFF)

set(PROTOZERO_GIT_TAG "v1.7.1")

CPMAddPackage(
  NAME           protozero
  GIT_REPOSITORY https://github.com/mapbox/protozero.git
  GIT_TAG        ${PROTOZERO_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/protozero/protozero.patch"
)

write_library_version("${PROTOZERO_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
