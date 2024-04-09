# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "openxr")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

if(BUILD_SHARED_LIBS)
  set(DYNAMIC_LOADER ON)
endif()

FetchContent_Declare(
  openxr
  GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
  GIT_TAG        release-1.0.34
)

FetchContent_MakeAvailable(openxr)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
