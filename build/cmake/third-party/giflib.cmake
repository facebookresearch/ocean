# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "giflib")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

set(GIFLIB_GIT_TAG "5.2.2")

CPMAddPackage(
    NAME           giflib
    GIT_REPOSITORY https://git.code.sf.net/p/giflib/code
    GIT_TAG        ${GIFLIB_GIT_TAG}
    PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/giflib/CMakeLists.txt" .
)

write_library_version("${GIFLIB_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
