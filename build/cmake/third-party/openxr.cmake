# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "openxr")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if(BUILD_SHARED_LIBS)
  set(DYNAMIC_LOADER ON)
else()
  set(DYNAMIC_LOADER OFF)
endif()

CPMAddPackage(
  NAME           openxr
  GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
  GIT_TAG        release-1.0.34
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
