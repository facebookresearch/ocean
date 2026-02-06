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
option(OCEAN_BUILD_MINIMAL "When enabled, build only base, cv, and math components of Ocean" OFF)

if (OCEAN_BUILD_MINIMAL)
    set(NOT_OCEAN_BUILD_MINIMAL FALSE)
else()
    set(NOT_OCEAN_BUILD_MINIMAL TRUE)
endif()


option(BUILD_SHARED_LIBS "Build using shared libraries" FALSE)
option(OCEAN_ENABLE_MESSENGER "When enabled, the logging mechanism in Ocean will be enabled. Otherwise it will be disabled" ${NOT_OCEAN_BUILD_MINIMAL})
option(OCEAN_BUILD_DEMOS "When enabled, the desktop demo applications will be included in the build. Otherwise they will be ignored." ${NOT_OCEAN_BUILD_MINIMAL})
option(OCEAN_BUILD_TESTS "When enabled, the desktop test libraries and applications will be included in the build. Otherwise they will be ignored." ${NOT_OCEAN_BUILD_MINIMAL})

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

# Third-party library layout support
# Supports both legacy "cmake" layout and new "python" layout from build_ocean_3p.py
set(OCEAN_THIRD_PARTY_LAYOUT "cmake" CACHE STRING "Third-party library layout: 'cmake' (legacy) or 'python' (new)")
set(OCEAN_THIRD_PARTY_ROOT "" CACHE PATH "Root directory for third-party libraries (required for 'python' layout)")

if (OCEAN_THIRD_PARTY_LAYOUT STREQUAL "python")
    if (NOT OCEAN_THIRD_PARTY_ROOT OR NOT EXISTS "${OCEAN_THIRD_PARTY_ROOT}")
        message(FATAL_ERROR "OCEAN_THIRD_PARTY_ROOT must be set to an existing directory when using OCEAN_THIRD_PARTY_LAYOUT=python")
    endif()

    # Determine platform string for headers (OS only, no arch)
    if (IOS)
        set(_ocean_3p_platform "ios")
    elseif (MACOS)
        set(_ocean_3p_platform "macos")
    elseif (ANDROID)
        set(_ocean_3p_platform "android")
    elseif (LINUX)
        set(_ocean_3p_platform "linux")
    elseif (WIN32)
        set(_ocean_3p_platform "windows")
    else()
        message(FATAL_ERROR "Unsupported platform for OCEAN_THIRD_PARTY_LAYOUT=python")
    endif()

    # Determine architecture string
    if (ANDROID)
        # Map Android ABI to our arch names
        if (ANDROID_ABI STREQUAL "arm64-v8a")
            set(_ocean_3p_arch "arm64")
        elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
            set(_ocean_3p_arch "armv7")
        elseif (ANDROID_ABI STREQUAL "x86_64")
            set(_ocean_3p_arch "x86_64")
        elseif (ANDROID_ABI STREQUAL "x86")
            set(_ocean_3p_arch "x86")
        else()
            message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}")
        endif()
    elseif (APPLE AND CMAKE_OSX_ARCHITECTURES)
        # On Apple platforms, CMAKE_OSX_ARCHITECTURES takes precedence over
        # CMAKE_SYSTEM_PROCESSOR for cross-compilation (e.g., x86_64 on ARM64 Mac)
        if (CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
            set(_ocean_3p_arch "arm64")
        elseif (CMAKE_OSX_ARCHITECTURES MATCHES "x86_64")
            set(_ocean_3p_arch "x86_64")
        else()
            message(FATAL_ERROR "Unsupported CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
        endif()
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
        set(_ocean_3p_arch "arm64")
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
        set(_ocean_3p_arch "x86_64")
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386")
        set(_ocean_3p_arch "x86")
    else()
        message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    # Determine link type
    if (BUILD_SHARED_LIBS)
        set(_ocean_3p_link "shared")
    else()
        set(_ocean_3p_link "static")
    endif()

    # Determine debug suffix
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_ocean_3p_config "_debug")
    else()
        set(_ocean_3p_config "")
    endif()

    # Build the target string: <platform>_<arch>_<link>[_debug]
    set(_ocean_3p_target "${_ocean_3p_platform}_${_ocean_3p_arch}_${_ocean_3p_link}${_ocean_3p_config}")

    message(STATUS "OCEAN_THIRD_PARTY_LAYOUT = python")
    message(STATUS "OCEAN_THIRD_PARTY_ROOT = ${OCEAN_THIRD_PARTY_ROOT}")
    message(STATUS "  Platform (headers): ${_ocean_3p_platform}")
    message(STATUS "  Target (libraries): ${_ocean_3p_target}")

    # Enumerate library directories and build search paths
    file(GLOB _ocean_3p_libs LIST_DIRECTORIES true "${OCEAN_THIRD_PARTY_ROOT}/*")

    foreach(_lib_dir ${_ocean_3p_libs})
        if (IS_DIRECTORY "${_lib_dir}")
            get_filename_component(_lib_name "${_lib_dir}" NAME)

            # Skip hidden directories
            if (_lib_name MATCHES "^\\.")
                continue()
            endif()

            # Add header path: <lib>/h/<platform>/
            set(_header_path "${_lib_dir}/h/${_ocean_3p_platform}")
            if (EXISTS "${_header_path}")
                list(APPEND CMAKE_INCLUDE_PATH "${_header_path}")
            endif()

            # Add library path: <lib>/lib/<target>/
            set(_lib_path "${_lib_dir}/lib/${_ocean_3p_target}")
            if (EXISTS "${_lib_path}")
                list(APPEND CMAKE_LIBRARY_PATH "${_lib_path}")
                list(APPEND CMAKE_PREFIX_PATH "${_lib_dir}")
            endif()
        endif()
    endforeach()

    # For Android, also set CMAKE_FIND_ROOT_PATH
    if (ANDROID)
        set(CMAKE_FIND_ROOT_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_LIBRARY_PATH})
    endif()

    # Add all 3P include paths to the global include directories
    # CMAKE_INCLUDE_PATH is only used by find_* commands, so we need to
    # explicitly add these paths for header-only libraries like vtzero/protozero
    include_directories(${CMAKE_INCLUDE_PATH})

    message(STATUS "  Include paths: ${CMAKE_INCLUDE_PATH}")
    message(STATUS "  Library paths: ${CMAKE_LIBRARY_PATH}")

    # Set explicit library hints for find_package modules that don't understand debug naming
    # The Python 3P build uses a 'd' suffix for debug libraries (e.g., libgifd.a vs libgif.a)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_debug_suffix "d")
    else()
        set(_debug_suffix "")
    endif()

    # GIF library - FindGIF.cmake doesn't understand debug naming
    set(_gif_lib_path "${OCEAN_THIRD_PARTY_ROOT}/giflib/lib/${_ocean_3p_target}")
    if (EXISTS "${_gif_lib_path}")
        if (WIN32)
            set(_gif_lib_name "gif${_debug_suffix}.lib")
        else()
            set(_gif_lib_name "libgif${_debug_suffix}.a")
        endif()
        if (EXISTS "${_gif_lib_path}/${_gif_lib_name}")
            set(GIF_LIBRARY "${_gif_lib_path}/${_gif_lib_name}" CACHE FILEPATH "GIF library" FORCE)
            message(STATUS "  GIF_LIBRARY = ${GIF_LIBRARY}")
        endif()
    endif()

    # PNG library - similar handling for debug naming
    set(_png_lib_path "${OCEAN_THIRD_PARTY_ROOT}/libpng/lib/${_ocean_3p_target}")
    if (EXISTS "${_png_lib_path}")
        if (WIN32)
            set(_png_lib_name "libpng16${_debug_suffix}.lib")
        else()
            set(_png_lib_name "libpng16${_debug_suffix}.a")
        endif()
        if (EXISTS "${_png_lib_path}/${_png_lib_name}")
            set(PNG_LIBRARY "${_png_lib_path}/${_png_lib_name}" CACHE FILEPATH "PNG library" FORCE)
            message(STATUS "  PNG_LIBRARY = ${PNG_LIBRARY}")
        endif()
    endif()

    # JPEG library - libjpeg-turbo uses 'jpeg' name
    set(_jpeg_lib_path "${OCEAN_THIRD_PARTY_ROOT}/libjpeg-turbo/lib/${_ocean_3p_target}")
    if (EXISTS "${_jpeg_lib_path}")
        if (WIN32)
            set(_jpeg_lib_name "jpeg${_debug_suffix}.lib")
        else()
            set(_jpeg_lib_name "libjpeg${_debug_suffix}.a")
        endif()
        if (EXISTS "${_jpeg_lib_path}/${_jpeg_lib_name}")
            set(JPEG_LIBRARY "${_jpeg_lib_path}/${_jpeg_lib_name}" CACHE FILEPATH "JPEG library" FORCE)
            message(STATUS "  JPEG_LIBRARY = ${JPEG_LIBRARY}")
        endif()
    endif()

    # TIFF library
    set(_tiff_lib_path "${OCEAN_THIRD_PARTY_ROOT}/libtiff/lib/${_ocean_3p_target}")
    if (EXISTS "${_tiff_lib_path}")
        if (WIN32)
            set(_tiff_lib_name "tiff${_debug_suffix}.lib")
        else()
            set(_tiff_lib_name "libtiff${_debug_suffix}.a")
        endif()
        if (EXISTS "${_tiff_lib_path}/${_tiff_lib_name}")
            set(TIFF_LIBRARY "${_tiff_lib_path}/${_tiff_lib_name}" CACHE FILEPATH "TIFF library" FORCE)
            message(STATUS "  TIFF_LIBRARY = ${TIFF_LIBRARY}")
        endif()
    endif()

    # ZLIB library
    set(_zlib_lib_path "${OCEAN_THIRD_PARTY_ROOT}/zlib/lib/${_ocean_3p_target}")
    if (EXISTS "${_zlib_lib_path}")
        if (WIN32)
            set(_zlib_lib_name "zlib${_debug_suffix}.lib")
        else()
            set(_zlib_lib_name "libz${_debug_suffix}.a")
        endif()
        if (EXISTS "${_zlib_lib_path}/${_zlib_lib_name}")
            set(ZLIB_LIBRARY "${_zlib_lib_path}/${_zlib_lib_name}" CACHE FILEPATH "ZLIB library" FORCE)
            message(STATUS "  ZLIB_LIBRARY = ${ZLIB_LIBRARY}")
        endif()
    endif()

    # GTest library
    set(_gtest_lib_path "${OCEAN_THIRD_PARTY_ROOT}/googletest/lib/${_ocean_3p_target}")
    if (EXISTS "${_gtest_lib_path}")
        if (WIN32)
            set(_gtest_lib_name "gtest${_debug_suffix}.lib")
            set(_gtest_main_lib_name "gtest_main${_debug_suffix}.lib")
        else()
            set(_gtest_lib_name "libgtest${_debug_suffix}.a")
            set(_gtest_main_lib_name "libgtest_main${_debug_suffix}.a")
        endif()
        if (EXISTS "${_gtest_lib_path}/${_gtest_lib_name}")
            set(GTEST_LIBRARY "${_gtest_lib_path}/${_gtest_lib_name}" CACHE FILEPATH "GTest library" FORCE)
            set(GTEST_MAIN_LIBRARY "${_gtest_lib_path}/${_gtest_main_lib_name}" CACHE FILEPATH "GTest main library" FORCE)
            set(GTEST_INCLUDE_DIR "${OCEAN_THIRD_PARTY_ROOT}/googletest/h/${_ocean_3p_platform}" CACHE PATH "GTest include dir" FORCE)
            message(STATUS "  GTEST_LIBRARY = ${GTEST_LIBRARY}")
            message(STATUS "  GTEST_MAIN_LIBRARY = ${GTEST_MAIN_LIBRARY}")
        endif()
    endif()

    # Freetype library
    set(_freetype_lib_path "${OCEAN_THIRD_PARTY_ROOT}/freetype/lib/${_ocean_3p_target}")
    if (EXISTS "${_freetype_lib_path}")
        if (WIN32)
            set(_freetype_lib_name "freetype${_debug_suffix}.lib")
        else()
            set(_freetype_lib_name "libfreetype${_debug_suffix}.a")
        endif()
        if (EXISTS "${_freetype_lib_path}/${_freetype_lib_name}")
            set(FREETYPE_LIBRARY "${_freetype_lib_path}/${_freetype_lib_name}" CACHE FILEPATH "Freetype library" FORCE)
            set(FREETYPE_INCLUDE_DIRS "${OCEAN_THIRD_PARTY_ROOT}/freetype/h/${_ocean_3p_platform}" CACHE PATH "Freetype include dir" FORCE)
            message(STATUS "  FREETYPE_LIBRARY = ${FREETYPE_LIBRARY}")
        endif()
    endif()

    # libusb library
    set(_libusb_lib_path "${OCEAN_THIRD_PARTY_ROOT}/libusb/lib/${_ocean_3p_target}")
    if (EXISTS "${_libusb_lib_path}")
        if (WIN32)
            set(_libusb_lib_name "usb${_debug_suffix}.lib")
        else()
            set(_libusb_lib_name "libusb${_debug_suffix}.a")
        endif()
        if (EXISTS "${_libusb_lib_path}/${_libusb_lib_name}")
            set(LIBUSB_1_LIBRARY "${_libusb_lib_path}/${_libusb_lib_name}" CACHE FILEPATH "libusb library" FORCE)
            set(LIBUSB_1_INCLUDE_DIR "${OCEAN_THIRD_PARTY_ROOT}/libusb/h/${_ocean_3p_platform}" CACHE PATH "libusb include dir" FORCE)
            message(STATUS "  LIBUSB_1_LIBRARY = ${LIBUSB_1_LIBRARY}")
        endif()
    endif()

    # Update macOS bundle library paths for Python layout
    # The paths set earlier (line ~88) use CMAKE_PREFIX_PATH which doesn't include
    # the target-specific subdirectory. For Python layout, use CMAKE_LIBRARY_PATH
    # which already contains the correct target-specific paths.
    if (MACOS AND BUILD_SHARED_LIBS)
        set(OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS ${CMAKE_LIBRARY_PATH})
        list(APPEND OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS "${CMAKE_INSTALL_PREFIX}/lib")
    endif()

    # Similarly update RPATH for shared builds
    if (BUILD_SHARED_LIBS)
        if (MACOS)
            set(CMAKE_INSTALL_RPATH ${CMAKE_LIBRARY_PATH})
            list(PREPEND CMAKE_INSTALL_RPATH @loader_path @loader_path/../lib)
        elseif (LINUX)
            set(CMAKE_INSTALL_RPATH ${CMAKE_LIBRARY_PATH})
            list(PREPEND CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/../lib)
        endif()
    endif()
endif()

message(STATUS " ")
message(STATUS "OCEAN_BUILD_MINIMAL = ${OCEAN_BUILD_MINIMAL}")
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
add_subdirectory(impl/ocean/math)
if (NOT OCEAN_BUILD_MINIMAL)
    add_subdirectory(impl/ocean/devices)
    add_subdirectory(impl/ocean/geometry)
    add_subdirectory(impl/ocean/interaction)
    add_subdirectory(impl/ocean/io)
    add_subdirectory(impl/ocean/media)
    add_subdirectory(impl/ocean/network)
    add_subdirectory(impl/ocean/platform)
    add_subdirectory(impl/ocean/rendering)
    add_subdirectory(impl/ocean/scenedescription)
    add_subdirectory(impl/ocean/system)
    add_subdirectory(impl/ocean/tracking)
endif()

# Demo applications
if (OCEAN_BUILD_DEMOS)
    add_subdirectory(impl/application/ocean/demo/base/console)
    add_subdirectory(impl/application/ocean/demo/base/sharedmemory/client)
    add_subdirectory(impl/application/ocean/demo/base/sharedmemory/server)
    add_subdirectory(impl/application/ocean/demo/cv/advanced/panoramaextractor)
    add_subdirectory(impl/application/ocean/demo/cv/calibration/creator)
    add_subdirectory(impl/application/ocean/demo/cv/calibration/offline)
    add_subdirectory(impl/application/ocean/demo/cv/calibration/picturetaker)
    add_subdirectory(impl/application/ocean/demo/cv/detector/barcodes/detector2d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/calibrationpatterngenerator)
    add_subdirectory(impl/application/ocean/demo/cv/detector/linedetector)
    add_subdirectory(impl/application/ocean/demo/cv/detector/messengercode)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/detector2d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/detector3d)
    add_subdirectory(impl/application/ocean/demo/cv/detector/qrcodes/qrcodegenerator)
    add_subdirectory(impl/application/ocean/demo/io/maps)
    add_subdirectory(impl/application/ocean/demo/io/serialization/inspector)
    add_subdirectory(impl/application/ocean/demo/media/imageextractor)
    add_subdirectory(impl/application/ocean/demo/media/imagereceiver)
    add_subdirectory(impl/application/ocean/demo/media/questcamera)
    add_subdirectory(impl/application/ocean/demo/media/videopreview)
    add_subdirectory(impl/application/ocean/demo/misc/appinspector)
    add_subdirectory(impl/application/ocean/demo/network/tcpclient)
    add_subdirectory(impl/application/ocean/demo/network/tcpserver)
    add_subdirectory(impl/application/ocean/demo/platform/android/pixelformats)
    add_subdirectory(impl/application/ocean/demo/media/externalcamera)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance)
    add_subdirectory(impl/application/ocean/demo/platform/meta/quest/openxr/depth)
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
    add_subdirectory(impl/application/ocean/demo/tracking/handtracker)
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
