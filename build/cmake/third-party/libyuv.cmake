# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libyuv")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
  NAME           libyuv
  GIT_REPOSITORY https://github.com/lemenkov/libyuv.git
  GIT_TAG        ce32eb773fd66239b794abee1e1ee53fdbc15bc0 # master as of 4/29/2024
  PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/libyuv/CMakeLists.txt" .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
