# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install path for headers")

add_library(gl INTERFACE)
target_include_directories(gl INTERFACE .)

install(DIRECTORY h/gl h/OpenGL DESTINATION "${INSTALL_INCLUDE_DIR}")
