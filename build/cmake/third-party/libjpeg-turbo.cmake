# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

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

set(LIBJPEG_TURBO_GIT_TAG "3.0.3")

CPMAddPackage(
  NAME                libjpeg-turbo
  GIT_REPOSITORY      https://github.com/libjpeg-turbo/libjpeg-turbo.git
  GIT_TAG             ${LIBJPEG_TURBO_GIT_TAG}
  PATCH_COMMAND       ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo/libjpeg-turbo.patch"
)

write_library_version("${LIBJPEG_TURBO_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
