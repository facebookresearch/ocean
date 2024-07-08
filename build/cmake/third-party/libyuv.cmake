# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "libyuv")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(JPEG_ROOT ${CMAKE_INSTALL_PREFIX})

find_package(Git REQUIRED)

CPMAddPackage(
  NAME           libyuv
  GIT_REPOSITORY https://github.com/lemenkov/libyuv.git
  GIT_TAG        ce32eb773fd66239b794abee1e1ee53fdbc15bc0 # master as of 4/29/2024
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libyuv/libyuv.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
