# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "openxr")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

# Ocean's Android/Quest OpenXR apps currently expect "openxr_loader" to be a shared library
# OpenXR's build process performs shared build of "openxr_loader" by default for all platforms except for Windows.
# Set "DYNAMIC_LOADER" variable to override this behavior.

include(GNUInstallDirs)

set(OPENXR_GIT_TAG "release-1.1.38")

CPMAddPackage(
  NAME           openxr
  GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
  GIT_TAG        ${OPENXR_GIT_TAG}
)

write_library_version("${OPENXR_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
