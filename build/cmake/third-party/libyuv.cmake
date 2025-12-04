# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "libyuv")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

get_library_install_root("libjpeg-turbo" JPEG_ROOT)

find_package(Git REQUIRED)

set(LIBYUV_GIT_TAG "ce32eb773fd66239b794abee1e1ee53fdbc15bc0") # master as of 4/29/2024

CPMAddPackage(
  NAME           libyuv
  GIT_REPOSITORY https://github.com/lemenkov/libyuv.git
  GIT_TAG        ${LIBYUV_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libyuv/libyuv.patch"
)

write_library_version("${LIBYUV_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
