# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

message(CHECK_START "wxwidgets")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if(BUILD_SHARED_LIBS)
  set(wxBUILD_SHARED ON CACHE BOOL "")
  set(wxWidgets_USE_STATIC OFF)
else()
  set(wxBUILD_SHARED OFF CACHE BOOL "")
  set(wxWidgets_USE_STATIC ON)
endif()

# Disable features/components not used in Ocean
set(wxUSE_WEBREQUEST OFF)

set(CMAKE_DEBUG_POSTFIX "")

CPMAddPackage(
  NAME           wxwidgets
  GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
  GIT_TAG        v3.2.4
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
