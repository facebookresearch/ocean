# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Discovers the android_native_app_glue header and static library produced
# by build_ocean_3rdparty.py via patches/android_native_app_glue_CMakeLists.txt
# (which builds the NDK source as `native_app_glue`, producing libnative_app_glue.a).
# Uses CMake's default search so the same Find module works for both 3P layouts:
#   - Standard layout: <prefix>/include/ and <prefix>/lib/ via CMAKE_PREFIX_PATH
#   - External-integration layout: via CMAKE_INCLUDE_PATH and CMAKE_LIBRARY_PATH
#     set up by the top-level CMakeLists.txt's external branch.

find_path(AndroidNativeAppGlue_INCLUDE_DIR
    NAMES android_native_app_glue.h
)

find_library(AndroidNativeAppGlue_LIBRARY
    NAMES native_app_glue android_native_app_glue
)

if(AndroidNativeAppGlue_INCLUDE_DIR AND AndroidNativeAppGlue_LIBRARY)
    set(AndroidNativeAppGlue_FOUND TRUE)

    message(STATUS "AndroidNativeAppGlue: ${AndroidNativeAppGlue_LIBRARY}")

    add_library(AndroidNativeAppGlue::AndroidNativeAppGlue STATIC IMPORTED)
    set_target_properties(AndroidNativeAppGlue::AndroidNativeAppGlue PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${AndroidNativeAppGlue_INCLUDE_DIR}
        IMPORTED_LOCATION ${AndroidNativeAppGlue_LIBRARY}
    )
endif()
