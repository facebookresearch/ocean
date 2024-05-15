# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "freetype")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(FT_DISABLE_BZIP2 TRUE)
set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})
set(PNG_ROOT ${CMAKE_INSTALL_PREFIX})

CPMAddPackage(
  NAME           freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG        VER-2-13-2
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
