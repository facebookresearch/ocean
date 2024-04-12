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

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})
set(PNG_ROOT  ${CMAKE_INSTALL_PREFIX})
set(tiff_ROOT ${CMAKE_INSTALL_PREFIX})
set(JPEG_ROOT ${CMAKE_INSTALL_PREFIX})

set(BUILD_ZLIB OFF CACHE BOOL "")
set(BUILD_PNG  OFF CACHE BOOL "")
set(BUILD_TIFF OFF CACHE BOOL "")
set(BUILD_JPEG OFF CACHE BOOL "")

set(BUILD_TESTS OFF CACHE BOOL "")
set(BUILD_PERF_TESTS OFF CACHE BOOL "")
set(BUILD_opencv_apps OFF CACHE BOOL "")

set(OPENCV_BIN_INSTALL_PATH "bin" CACHE BOOL "")
set(OPENCV_LIB_INSTALL_PATH "lib" CACHE BOOL "")
set(OPENCV_LIB_ARCHIVE_INSTALL_PATH "lib" CACHE BOOL "")

FetchContent_Declare(
  opencv
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG        4.9.0
)

FetchContent_MakeAvailable(opencv)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
