# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "assimp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})

option(BUILD_SHARED_LIBS "" BUILD_SHARED_LIBS)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.3.1
  PATCH_COMMAND  ${CMAKE_COMMAND} -E remove ./cmake-modules/FindZLIB.cmake
)

set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "")

FetchContent_MakeAvailable(assimp)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
