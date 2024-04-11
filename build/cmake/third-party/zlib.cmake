# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "zlib")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf # 1.3.1
    PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/zlib/CMakeLists.txt" .
)

FetchContent_MakeAvailable(zlib)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
