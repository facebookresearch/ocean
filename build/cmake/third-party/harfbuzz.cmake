# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "harfbuzz")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if(IOS)
  set(HB_HAVE_CORETEXT OFF)
endif()

CPMAddPackage(
  NAME           harfbuzz
  GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
  GIT_TAG        8.4.0
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
