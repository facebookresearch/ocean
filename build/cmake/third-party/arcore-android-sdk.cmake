# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "arcore-android-sdk")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
  NAME           arcore-android-sdk
  SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/arcore-android-sdk"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
