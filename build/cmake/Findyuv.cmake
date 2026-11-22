# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Finds libyuv as installed by build_ocean_3rdparty.py (see
# build/python/patches/libyuv.patch): the library and the headers, without a
# CMake package configuration. The library is found through CMAKE_PREFIX_PATH
# in the standard third-party layout and through CMAKE_INCLUDE_PATH /
# CMAKE_LIBRARY_PATH in the external-integration layout.
#
# Defines the imported target yuv::yuv and the variables
#
#   yuv_FOUND
#   yuv_INCLUDE_DIR - the directory containing libyuv.h
#   yuv_LIBRARY     - the libyuv library
#
# libyuv is compiled with HAVE_JPEG when a JPEG library is available, so a
# static libyuv needs JPEG at link time. The imported target carries it when
# JPEG can be found.

find_path(yuv_INCLUDE_DIR NAMES libyuv.h)
find_library(yuv_LIBRARY NAMES yuv)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yuv REQUIRED_VARS yuv_LIBRARY yuv_INCLUDE_DIR)

if (yuv_FOUND AND NOT TARGET yuv::yuv)
    add_library(yuv::yuv UNKNOWN IMPORTED)
    set_target_properties(yuv::yuv PROPERTIES
        IMPORTED_LOCATION "${yuv_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${yuv_INCLUDE_DIR}"
    )

    find_package(JPEG QUIET)
    if (JPEG_FOUND)
        set_property(TARGET yuv::yuv APPEND PROPERTY INTERFACE_LINK_LIBRARIES JPEG::JPEG)
    else()
        message(STATUS "libyuv found without JPEG; linking fails if libyuv was built with HAVE_JPEG")
    endif()
endif()

mark_as_advanced(yuv_INCLUDE_DIR yuv_LIBRARY)
