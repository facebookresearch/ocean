# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "assimp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})
set(JPEG_ROOT ${CMAKE_INSTALL_PREFIX})

if(BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "" ON)
else()
  option(BUILD_SHARED_LIBS "" OFF)
  # .pdb file is an output of the linker which doesn't run
  option(ASSIMP_INSTALL_PDB "" OFF)
endif()

set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")

if (LINUX)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    # Avoid errors on certain Linux builds
    add_compile_options(-Wno-error=array-bounds)
  endif()
endif()

CPMAddPackage(
  NAME           assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.4.2
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/assimp/assimp.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
