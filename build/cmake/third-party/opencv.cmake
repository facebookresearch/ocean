# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "opencv")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

cmake_policy(SET CMP0144 NEW)

if(BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "" ON)
else()
  option(BUILD_SHARED_LIBS "" OFF)
endif()

set(ZLIB_ROOT /c/tmp/ocean_third-party_install_release)
set(PNG_ROOT /c/tmp/ocean_third-party_install_release)
set(tiff_ROOT /c/tmp/ocean_third-party_install_release)

set(BUILD_ZLIB OFF CACHE BOOL "")
set(BUILD_PNG OFF CACHE BOOL "")
set(BUILD_TIFF OFF CACHE BOOL "")

FetchContent_Declare(
  opencv
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG        4.9.0
)

FetchContent_MakeAvailable(opencv)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
