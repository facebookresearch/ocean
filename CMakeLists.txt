# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

# Make option() honor normal variables
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # For CMake subprojects

if (APPLE)
    # Workaround to allow shared libraries to be found via RPATH.
    # "-install_name @rpath/..." linker option not specified for Objective-C++ libraries otherwise
    set(CMAKE_SHARED_LIBRARY_SONAME_OBJC_FLAG "-install_name")
endif()

if(CMAKE_SYSTEM_NAME AND "${CMAKE_SYSTEM_NAME}" STREQUAL Android)
  include(build/cmake/FindNinjaAndroidSDK.cmake)
endif()

project(ocean)

# Additional CMake configurations
include(${CMAKE_CURRENT_LIST_DIR}/build/cmake/ocean_utilities.cmake)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/build/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/build/cmake")

if (ANDROID)
    list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/build/cmake")
    set(CMAKE_STAGING_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

if (IOS)
    enable_language(Swift)

    if (NOT DEFINED CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY)
        set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
    endif()

    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM)
        message(WARNING "You need to specify your own Apple Team ID when configurating with CMake: -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX")
    endif()
endif()

# Build options
option(BUILD_SHARED_LIBS "Build using shared libraries" FALSE)
option(OCEAN_ENABLE_MESSENGER "When enabled, the logging mechanism in Ocean will be enabled. Otherwise it will be disabled" TRUE)
option(OCEAN_BUILD_DEMOS "When enabled, the desktop demo applications will be included in the build. Otherwise they will be ignored." TRUE)
option(OCEAN_BUILD_TESTS "When enabled, the desktop test libraries and applications will be included in the build. Otherwise they will be ignored." TRUE)

# Build config and definitions
set(CMAKE_CXX_STANDARD 20)

set(OCEAN_IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/impl")

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

get_ocean_preprocessor_flags(OCEAN_PREPROCESSOR_FLAGS)
get_ocean_compiler_flags(OCEAN_COMPILER_FLAGS)

if (IOS OR MACOS)
    # Enable Automatic Reference Counting (-fobjc-arc).  Disabling will result in memory leaks in some builds.
    # ARC adds to exception handling code by default for Objective-C++.  Disabling here (-fno-objc-arc-exceptions) to reduce binary size.  Use "-fobjc-arc-exceptions" to override.
    string(APPEND CMAKE_OBJC_FLAGS " -fobjc-arc")
    string(APPEND CMAKE_OBJCXX_FLAGS " -fobjc-arc -fno-objc-arc-exceptions")
endif()

if (MACOS)
    # Assemble list of directories which contain Ocean libraries and third party libraries into "OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS".
    # When installing executables as ".app" bundles for MacOS, "fixup_bundle()" provided by CMake's BuntleUtilities module
    # is used to install shared libraries needed by the application into the "*.app/" directory structure.
    # "OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS" is passed to "fixup_bundle()" to specify where the shared library files can be found.
    list(TRANSFORM CMAKE_PREFIX_PATH APPEND /lib OUTPUT_VARIABLE OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS)
    list(APPEND OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS "${CMAKE_INSTALL_PREFIX}/lib")
endif()

if (BUILD_SHARED_LIBS)
    if (MACOS OR LINUX)
        list(TRANSFORM CMAKE_PREFIX_PATH APPEND /lib OUTPUT_VARIABLE CMAKE_INSTALL_RPATH)

        if (MACOS)
            list(PREPEND CMAKE_INSTALL_RPATH @loader_path @loader_path/../lib)
        elseif (LINUX)
            list(PREPEND CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/../lib)
        endif()
    endif()
endif()

if (ANDROID)
    # If specified, use the following variable to determine the location of the third-party
    # libraries that are required. This is provided by Gradle.
    if (OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE)
        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(CMAKE_FIND_ROOT_PATH ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}/android_${ANDROID_ABI}_static_Debug)
        else()
            set(CMAKE_FIND_ROOT_PATH ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}/android_${ANDROID_ABI}_static_Release)
        endif()

        message(STATUS "OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE = ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}")
    else()
        message(STATUS "OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE = [not defined]")
    endif()
endif()

message(STATUS " ")
message(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OCEAN_COMPILER_FLAGS = \"${OCEAN_COMPILER_FLAGS}\"")
message(STATUS "OCEAN_ENABLE_MESSENGER = ${OCEAN_ENABLE_MESSENGER}")
message(STATUS "OCEAN_HOST_PROCESSOR_TYPE = ${OCEAN_HOST_PROCESSOR_TYPE}")
message(STATUS "OCEAN_HOST_SYSTEM_NAME = ${OCEAN_HOST_SYSTEM_NAME}")
message(STATUS "OCEAN_IMPL_DIR = ${OCEAN_IMPL_DIR}")
message(STATUS "OCEAN_PREPROCESSOR_FLAGS = \"${OCEAN_PREPROCESSOR_FLAGS}\"")
message(STATUS "OCEAN_TARGET_PROCESSOR_TYPE = ${OCEAN_TARGET_PROCESSOR_TYPE}")
message(STATUS "OCEAN_TARGET_SYSTEM_NAME = ${OCEAN_TARGET_SYSTEM_NAME}")
message(STATUS " ")

# Ocean libraries
add_subdirectory(impl/ocean/base)
add_subdirectory(impl/ocean/cv)
add_subdirectory(impl/ocean/devices)
add_subdirectory(impl/ocean/geometry)
add_subdirectory(impl/ocean/interaction)
add_subdirectory(impl/ocean/io)
add_subdirectory(impl/ocean/math)
add_subdirectory(impl/ocean/media)
add_subdirectory(impl/ocean/network)
add_subdirectory(impl/ocean/platform)
add_subdirectory(impl/ocean/rendering)
add_subdirectory(impl/ocean/scenedescription)
add_subdirectory(impl/ocean/system)
add_subdirectory(impl/ocean/tracking)

# Demo applications
if (OCEAN_BUILD_DEMOS)
    add_subdirectory(impl/application/ocean/demo/base/console)
    add_subdirectory(impl/application/ocean/demo/base/sharedmemory/client)
    add_subdirectory(impl/application/ocean/demo/base/sharedmemory/server)
    add_subdirectory(impl/application/ocean/demo/cv/advanced/panoramaextractor)
    add_subdirectory(impl/application/ocean/demo/cv/detector/barcodes/detector2d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/linedetector)
    add_subdirectory(impl/application/ocean/demo/cv/detector/messengercode)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/detector2d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/detector3d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/qrcodegenerator)
    add_subdirectory(impl/application/ocean/demo/io/maps)
    add_subdirectory(impl/application/ocean/demo/media/imageextractor)
    add_subdirectory(impl/application/ocean/demo/media/videopreview)
    add_subdirectory(impl/application/ocean/demo/misc/appinspector)
    add_subdirectory(impl/application/ocean/demo/misc/imageannotator)
    add_subdirectory(impl/application/ocean/demo/network/tcpclient)
    add_subdirectory(impl/application/ocean/demo/network/tcpserver)
    add_subdirectory(impl/application/ocean/demo/platform/android/pixelformats)
    add_subdirectory(impl/application/ocean/demo/media/externalcamera)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/graphs)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/handgestures)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/microphone)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/panoramaviewer)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/renderer)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/solarsystem)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/theremin)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplication)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplicationadvanced)
    add_subdirectory(impl/application/ocean/demo/platform/osxobjc)
    add_subdirectory(impl/application/ocean/demo/tracking/featuretracker)
    add_subdirectory(impl/application/ocean/demo/tracking/homographyimagealigner)
    add_subdirectory(impl/application/ocean/demo/tracking/homographytracker)
    add_subdirectory(impl/application/ocean/demo/tracking/oculustags/oculustaggenerator)
    add_subdirectory(impl/application/ocean/demo/tracking/oculustags/oculustagtracker)
    add_subdirectory(impl/application/ocean/demo/tracking/offline/pointpathsextractor)
    add_subdirectory(impl/application/ocean/demo/tracking/pointtracker)
    add_subdirectory(impl/application/ocean/demo/tracking/similaritytracker)
    add_subdirectory(impl/application/ocean/demo/tracking/stereomatcher)
    add_subdirectory(impl/application/ocean/shark)
endif()

if (OCEAN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(impl/ocean/test)
    add_subdirectory(impl/application/ocean/test)
endif()
