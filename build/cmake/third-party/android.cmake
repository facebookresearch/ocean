# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

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
