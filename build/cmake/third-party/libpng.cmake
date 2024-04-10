# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libpng")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG        ed217e3e601d8e462f7fd1e04bed43ac42212429 # 1.6.43
)
FetchContent_MakeAvailable(libpng)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
