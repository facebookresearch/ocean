# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "freetype")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG        VER-2-13-2
)

FetchContent_MakeAvailable(freetype)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
