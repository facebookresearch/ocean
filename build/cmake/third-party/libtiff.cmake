# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "libtiff")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(tiff-install ON CACHE BOOL "")
set(tiff-tools OFF CACHE BOOL "")
set(tiff-tests OFF CACHE BOOL "")
set(tiff-contrib OFF CACHE BOOL "")
set(HAVE_LD_VERSION_SCRIPT OFF)

# Explicitly disable the following codes
set(libdeflate OFF)
set(lzma OFF)
set(jbig OFF)
set(webp OFF)
set(zstd OFF)
set(lerc OFF)

find_package(Git REQUIRED)

set(LIBTIFF_GIT_TAG "v4.6.0")

CPMAddPackage(
  NAME           libtiff
  GIT_REPOSITORY https://github.com/libsdl-org/libtiff.git
  GIT_TAG        ${LIBTIFF_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libtiff/libtiff.patch"
)

write_library_version("${LIBTIFF_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
