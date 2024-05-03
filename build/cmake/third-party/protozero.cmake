# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "protozero")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

CPMAddPackage(
  NAME           protozero
  GIT_REPOSITORY https://github.com/mapbox/protozero.git
  GIT_TAG        v1.7.1
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/protozero/protozero.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
