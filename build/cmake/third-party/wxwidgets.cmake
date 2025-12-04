# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)

message(CHECK_START "wxwidgets")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

get_library_install_root("zlib" ZLIB_ROOT)
get_library_install_root("libpng" PNG_ROOT)
get_library_install_root("libjpeg-turbo" JPEG_ROOT)
get_library_install_root("libtiff" TIFF_ROOT)

if(BUILD_SHARED_LIBS)
  set(wxBUILD_SHARED ON CACHE BOOL "")
  set(wxWidgets_USE_STATIC OFF)
else()
  set(wxBUILD_SHARED OFF CACHE BOOL "")
  set(wxWidgets_USE_STATIC ON)
endif()

# Configure wxWidgets to use external libraries instead of bundled ones
set(wxUSE_LIBJPEG "sys" CACHE STRING "")
set(wxUSE_LIBPNG "sys" CACHE STRING "")
set(wxUSE_ZLIB "sys" CACHE STRING "")
set(wxUSE_LIBTIFF "sys" CACHE STRING "")

# Disable features/components not used in Ocean
set(wxUSE_WEBREQUEST OFF)

set(CMAKE_DEBUG_POSTFIX "")

set(WXWIDGETS_GIT_TAG "v3.3.1")

CPMAddPackage(
  NAME           wxwidgets
  GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
  GIT_TAG        ${WXWIDGETS_GIT_TAG}
)

write_library_version("${WXWIDGETS_GIT_TAG}")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
