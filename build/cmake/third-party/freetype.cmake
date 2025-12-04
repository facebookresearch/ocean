# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "freetype")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(FT_DISABLE_BROTLI TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)

get_library_install_root("zlib" ZLIB_ROOT)
get_library_install_root("libpng" PNG_ROOT)

set(FREETYPE_GIT_TAG "VER-2-13-2")

CPMAddPackage(
  NAME           freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG        ${FREETYPE_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/freetype/freetype.patch"
)

write_library_version("${FREETYPE_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
