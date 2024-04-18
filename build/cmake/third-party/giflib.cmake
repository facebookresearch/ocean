# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "giflib")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
    NAME           giflib
    GIT_REPOSITORY https://git.code.sf.net/p/giflib/code
    GIT_TAG        5.2.2
    PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/giflib/CMakeLists.txt" .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
