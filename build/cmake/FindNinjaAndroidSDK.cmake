# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This module finds Ninja for Android builds.
# It first checks if CMAKE_MAKE_PROGRAM is already set (e.g., from command line),
# then looks in ANDROID_HOME, and finally falls back to find_program.

# If CMAKE_MAKE_PROGRAM is already set and valid, use it
if(CMAKE_MAKE_PROGRAM AND EXISTS "${CMAKE_MAKE_PROGRAM}")
  return()
endif()

if(NOT DEFINED ENV{ANDROID_HOME})
  message(FATAL_ERROR "Error: Environment variable ANDROID_HOME is not set")
endif()

if(WIN32)
  set(EXE_NAME "ninja.exe")
else()
  set(EXE_NAME "ninja")
endif()

# Try to find Ninja in Android SDK first
set(NPAT "$ENV{ANDROID_HOME}/${EXE_NAME}")
file(GLOB_RECURSE NINJA_CANDIDATES "${NPAT}")

if(NINJA_CANDIDATES)
  list(POP_FRONT NINJA_CANDIDATES CMP)
  set(CMAKE_MAKE_PROGRAM "${CMP}" CACHE PATH "" FORCE)
else()
  # Fall back to finding Ninja in system PATH
  find_program(NINJA_EXECUTABLE ninja)
  if(NINJA_EXECUTABLE)
    set(CMAKE_MAKE_PROGRAM "${NINJA_EXECUTABLE}" CACHE PATH "" FORCE)
  else()
    message(FATAL_ERROR "Could not find Ninja. Please install Ninja and ensure it's in your PATH.")
  endif()
endif()