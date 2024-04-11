# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "assimp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

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

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.4.0
  PATCH_COMMAND  ${CMAKE_COMMAND} -E remove ./cmake-modules/FindZLIB.cmake
)

FetchContent_MakeAvailable(assimp)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
