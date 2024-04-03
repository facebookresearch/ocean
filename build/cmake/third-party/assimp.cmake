# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "assimp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

#set(ZLIB_ROOT /c/tmp/ocean_third-party_install_release)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.3.1
)

set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "")

FetchContent_MakeAvailable(assimp)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
