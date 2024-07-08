# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

find_path(AndroidNativeAppGlue_INCLUDE_DIR
    NAMES android_native_app_glue.h
    PATHS ${CMAKE_MODULE_PATH}/include
)

find_library(AndroidNativeAppGlue_LIBRARY
    NAMES libandroid_native_app_glue.a
    PATHS ${CMAKE_MODULE_PATH}/lib
)

if(AndroidNativeAppGlue_INCLUDE_DIR AND AndroidNativeAppGlue_LIBRARY)
    set(AndroidNativeAppGlue_FOUND TRUE)

    message(STATUS "AndroidAppGlue_FOUND")

    add_library(AndroidNativeAppGlue::AndroidNativeAppGlue STATIC IMPORTED)
    set_target_properties(AndroidNativeAppGlue::AndroidNativeAppGlue PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${AndroidNativeAppGlue_INCLUDE_DIR}
        IMPORTED_LOCATION ${AndroidNativeAppGlue_LIBRARY}
    )
endif()
