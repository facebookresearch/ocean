# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "directshow")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

# DirectShow is Windows-only
if(NOT WIN32)
  message(CHECK_FAIL "skipped (Windows only)")
  list(POP_BACK CMAKE_MESSAGE_INDENT)
  return()
endif()

set(DIRECTSHOW_GIT_TAG "adf93f0")

CPMAddPackage(
  NAME                directshow
  GIT_REPOSITORY      https://github.com/microsoft/Windows-classic-samples.git
  GIT_TAG             ${DIRECTSHOW_GIT_TAG}
  SOURCE_SUBDIR       Samples/Win7Samples/multimedia/directshow/baseclasses
  PATCH_COMMAND       ${CMAKE_COMMAND} -E copy
                      "${CMAKE_CURRENT_SOURCE_DIR}/directshow/CMakeLists.txt"
                      "<SOURCE_DIR>/Samples/Win7Samples/multimedia/directshow/baseclasses/CMakeLists.txt"
)

write_library_version("${DIRECTSHOW_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
