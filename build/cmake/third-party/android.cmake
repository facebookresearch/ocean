# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "android")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
  android
  URL ${CMAKE_CURRENT_LIST_DIR}/android
)

FetchContent_MakeAvailable(android)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
