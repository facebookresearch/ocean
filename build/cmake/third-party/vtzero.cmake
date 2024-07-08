# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "vtzero")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(PROTOZERO_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")

find_package(Git REQUIRED)

CPMAddPackage(
  NAME           vtzero
  GIT_REPOSITORY https://github.com/mapbox/vtzero.git
  GIT_TAG        v1.1.0
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/vtzero/vtzero.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
