# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "vtzero")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(PROTOZERO_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")

CPMAddPackage(
  NAME           vtzero
  GIT_REPOSITORY https://github.com/mapbox/vtzero.git
  GIT_TAG        v1.1.0
  PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/vtzero/CMakeLists.txt" .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
