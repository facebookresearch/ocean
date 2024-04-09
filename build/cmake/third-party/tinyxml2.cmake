# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "tinyxml2")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG        10.0.0
)

FetchContent_MakeAvailable(tinyxml2)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
