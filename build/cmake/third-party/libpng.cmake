# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "libpng")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})

if(BUILD_SHARED_LIBS)
  set(PNG_SHARED ON CACHE BOOL "")
  set(PNG_STATIC OFF CACHE BOOL "")
else()
  set(PNG_SHARED OFF CACHE BOOL "")
  set(PNG_STATIC ON CACHE BOOL "")
endif()

set(PNG_TOOLS OFF CACHE BOOL "")
set(PNG_EXECUTABLES OFF)
set(PNG_FRAMEWORK OFF)
set(PNG_DEBUG_POSTFIX "")

CPMAddPackage(
  NAME           libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG        ed217e3e601d8e462f7fd1e04bed43ac42212429 # 1.6.43
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libpng/libpng.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
