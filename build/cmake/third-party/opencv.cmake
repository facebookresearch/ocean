# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "opencv")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

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

CPMAddPackage(
  NAME           opencv_contrib
  GIT_REPOSITORY https://github.com/opencv/opencv_contrib.git
  GIT_TAG        4.9.0
  DOWNLOAD_ONLY
)

set(OPENCV_EXTRA_MODULES_PATH "${opencv_contrib_SOURCE_DIR}/modules")

CPMAddPackage(
  NAME           opencv
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG        4.9.0
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
