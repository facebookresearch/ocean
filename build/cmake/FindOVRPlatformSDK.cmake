# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

find_path(OVRPlatformSDK_INCLUDE_DIR
    NAMES OVR_Platform.h
    PATH_SUFFIXES Include
    NO_CMAKE_FIND_ROOT_PATH
)

if (ANDROID)
    find_library(OVRPlatformSDK_LIBRARY
        NAMES ovrplatformloader
        PATH_SUFFIXES Android/libs/${CMAKE_ANDROID_ARCH_ABI}
        NO_CMAKE_FIND_ROOT_PATH
    )
elseif (WIN32)
    message(STATUS "OVRPlatformSDK not found: Not yet configured for Windows")
    return()
else()
    message(STATUS "OVRPlatformSDK not found: Only available for Android and Windows")
    return()
endif()

if(OVRPlatformSDK_INCLUDE_DIR AND OVRPlatformSDK_LIBRARY)
    set(OVRPlatformSDK_FOUND TRUE)

    message(STATUS "OVRPlatformSDK_FOUND")

    add_library(OVRPlatformSDK::OVRPlatformSDK SHARED IMPORTED)
    set_target_properties(OVRPlatformSDK::OVRPlatformSDK PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${OVRPlatformSDK_INCLUDE_DIR}
        IMPORTED_LOCATION ${OVRPlatformSDK_LIBRARY}
    )
elseif (NOT DEFINED ENV{OVRPlatformSDK_ROOT})
    message(STATUS "Attempt to find package 'Oculus (OVR) Platform SDK' failed.  Possible cause: missing environment variable 'OVRPlatformSDK_ROOT'")
endif()
