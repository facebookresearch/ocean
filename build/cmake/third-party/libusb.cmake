# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "libusb")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

CPMAddPackage(
  NAME           libusb
  GIT_REPOSITORY https://github.com/libusb/libusb-cmake.git
  GIT_TAG        cec1e49eec481a6333c218726ecaba8e4f447a07
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
