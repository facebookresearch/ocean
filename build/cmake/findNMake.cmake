# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

file(GLOB_RECURSE NMAKE_CANDIDATES "C:/Program Files/Microsoft Visual Studio/nmake.exe")
foreach(CAND ${NMAKE_CANDIDATES})
  message(STATUS ${CAND})
  string(REGEX MATCH Hostx64/x64 MATCHOUT ${CAND})
  if(MATCHOUT)
    set(CMAKE_MAKE_PROGRAM ${CAND})
    break()
  endif()
endforeach()
