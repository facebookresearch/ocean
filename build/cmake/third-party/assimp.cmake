# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "assimp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

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

CPMAddPackage(
  NAME           assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.4.0
  PATCH_COMMAND  ${CMAKE_COMMAND} -E remove ./cmake-modules/FindZLIB.cmake
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
