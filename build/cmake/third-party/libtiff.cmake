# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libtiff")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(tiff-install ON CACHE BOOL "")
set(tiff-tools OFF CACHE BOOL "")
set(tiff-tests OFF CACHE BOOL "")
set(tiff-contrib OFF CACHE BOOL "")

CPMAddPackage(
  NAME           libtiff
  GIT_REPOSITORY https://github.com/libsdl-org/libtiff.git
  GIT_TAG        v4.6.0
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
