# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
cmake_minimum_required(VERSION 3.26)

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install path for headers")

add_library(android INTERFACE)
target_include_directories(android INTERFACE .)

install(DIRECTORY android DESTINATION "${INSTALL_INCLUDE_DIR}")

