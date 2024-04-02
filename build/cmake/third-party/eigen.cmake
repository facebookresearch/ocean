# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
	
message(CHECK_START "eigen")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        287c8017808fb4c7a651ba4c02363a773e2f0c46 # 3.4
)

set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)

FetchContent_MakeAvailable(eigen)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")