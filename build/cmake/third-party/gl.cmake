# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "gl")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
  gl
  URL ${CMAKE_CURRENT_LIST_DIR}/gl
)

FetchContent_MakeAvailable(gl)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
