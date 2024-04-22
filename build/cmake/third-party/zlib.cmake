# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "zlib")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
  NAME                zlib
  GIT_REPOSITORY      https://github.com/madler/zlib.git
  GIT_TAG             51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf
  PATCH_COMMAND       ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/zlib/CMakeLists.txt" .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
