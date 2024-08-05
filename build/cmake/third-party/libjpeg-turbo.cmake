# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libjpeg-turbo")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

if(BUILD_SHARED_LIBS)
  option(ENABLE_SHARED "" TRUE)
  option(ENABLE_STATIC "" FALSE)
else()
  option(ENABLE_SHARED "" FALSE)
  option(ENABLE_STATIC "" TRUE)
endif()

option(WITH_TURBOJPEG FALSE)

CPMAddPackage(
  NAME                libjpeg-turbo
  GIT_REPOSITORY      https://github.com/libjpeg-turbo/libjpeg-turbo.git
  GIT_TAG             3.0.3
  PATCH_COMMAND       ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo/libjpeg-turbo.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
