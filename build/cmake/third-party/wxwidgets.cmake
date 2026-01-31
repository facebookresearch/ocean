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

# Patch wxWidgetsConfig.cmake to add missing find_package calls for ZLIB and JPEG.
# These are required because libtiff's CMake config references ZLIB::ZLIB and JPEG::JPEG
# targets, but wxWidgetsConfig.cmake doesn't call find_package() to create them.
install(CODE "
    set(WX_CONFIG_FILE \"\${CMAKE_INSTALL_PREFIX}/lib/cmake/wxWidgets/wxWidgetsConfig.cmake\")
    if(EXISTS \"\${WX_CONFIG_FILE}\")
        file(READ \"\${WX_CONFIG_FILE}\" WX_CONFIG_CONTENT)

        # Check if the patch has already been applied
        if(NOT WX_CONFIG_CONTENT MATCHES \"find_package.ZLIB QUIET.\")
            # Find the insertion point: after the OpenGL find_package block
            string(REPLACE
                \"find_package(Threads QUIET)\"
                \"# make sure ZLIB and JPEG targets are available
# (required for libtiff dependencies in wxcore)
if(TARGET wx::wxcore)
    find_package(ZLIB QUIET)
    find_package(JPEG QUIET)
endif()

find_package(Threads QUIET)\"
                WX_CONFIG_CONTENT
                \"\${WX_CONFIG_CONTENT}\"
            )

            file(WRITE \"\${WX_CONFIG_FILE}\" \"\${WX_CONFIG_CONTENT}\")
            message(STATUS \"Patched wxWidgetsConfig.cmake to add ZLIB and JPEG find_package calls\")
        else()
            message(STATUS \"wxWidgetsConfig.cmake already patched\")
        endif()
    else()
        message(WARNING \"wxWidgetsConfig.cmake not found at \${WX_CONFIG_FILE}\")
    endif()
")

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
