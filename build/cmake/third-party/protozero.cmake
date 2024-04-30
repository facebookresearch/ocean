# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "protozero")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
  NAME           protozero
  GIT_REPOSITORY https://github.com/mapbox/protozero.git
  GIT_TAG        v1.7.1
  PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/protozero/CMakeLists.txt" .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
