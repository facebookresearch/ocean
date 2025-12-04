# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "gl")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

set(GL_VERSION "local")

FetchContent_Declare(
  gl
  URL ${CMAKE_CURRENT_LIST_DIR}/gl
)

FetchContent_MakeAvailable(gl)

write_library_version("${GL_VERSION}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
