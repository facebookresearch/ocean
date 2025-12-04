# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "libpng")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

get_library_install_root("zlib" ZLIB_ROOT)

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

set(LIBPNG_GIT_TAG "ed217e3e601d8e462f7fd1e04bed43ac42212429") # 1.6.43

CPMAddPackage(
  NAME           libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG        ${LIBPNG_GIT_TAG}
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libpng/libpng.patch"
)

write_library_version("${LIBPNG_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
