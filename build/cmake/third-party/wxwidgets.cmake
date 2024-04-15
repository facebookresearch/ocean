# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "wxwidgets")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

if(BUILD_SHARED_LIBS)
  set(wxBUILD_SHARED ON CACHE BOOL "")
else()
  set(wxBUILD_SHARED OFF CACHE BOOL "")
  set(wxWidgets_USE_STATIC ON)
endif()

FetchContent_Declare(
  wxwidgets
  GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
  GIT_TAG        v3.2.4
)

FetchContent_MakeAvailable(wxwidgets)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
