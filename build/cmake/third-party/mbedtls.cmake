# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "mbedtls")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(ENABLE_TESTING OFF)
set(ENABLE_PROGRAMS OFF)

find_package(Git REQUIRED)

CPMAddPackage(
  NAME           mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
  GIT_TAG        v3.5.2
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/mbedtls.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
