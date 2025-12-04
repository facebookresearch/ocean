# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "zlib")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(ZLIB_BUILD_EXAMPLES OFF)

set(ZLIB_GIT_TAG "51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf")

CPMAddPackage(
  NAME                zlib
  GIT_REPOSITORY      https://github.com/madler/zlib.git
  GIT_TAG             ${ZLIB_GIT_TAG}
  PATCH_COMMAND       ${GIT_EXECUTABLE} apply --ignore-whitespace "${CMAKE_CURRENT_SOURCE_DIR}/zlib/zlib.patch"
)

write_library_version("${ZLIB_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
