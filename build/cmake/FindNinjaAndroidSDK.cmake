# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if(NOT DEFINED ENV{ANDROID_HOME})
  message(FATAL_ERROR "Error: Environment variable ANDROID_HOME is not set")
endif()

if(WIN32)
  set(EXE_NAME "ninja.exe")
else()
  set(EXE_NAME "ninja")
endif()

set(NPAT "$ENV{ANDROID_HOME}/${EXE_NAME}")

file(GLOB_RECURSE NINJA_CANDIDATES "${NPAT}")

list(POP_FRONT NINJA_CANDIDATES CMP)

set(CMAKE_MAKE_PROGRAM "${CMP}" CACHE PATH "" FORCE)
