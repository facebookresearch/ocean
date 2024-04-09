# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
cmake_minimum_required(VERSION 3.26)

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install path for headers")

add_library(gl INTERFACE)
target_include_directories(gl INTERFACE .)

install(DIRECTORY h/gl h/OpenGL DESTINATION "${INSTALL_INCLUDE_DIR}")

