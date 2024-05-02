# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

project(ocean_third_party)

include(${CMAKE_CURRENT_LIST_DIR}/../ocean_utilities.cmake)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

ocean_translate_system_name(${CMAKE_HOST_SYSTEM_NAME} OCEAN_HOST_SYSTEM_NAME)
ocean_translate_system_name(${CMAKE_SYSTEM_NAME} OCEAN_TARGET_SYSTEM_NAME)

set(OCEAN_HOST_PROCESSOR_TYPE ${CMAKE_HOST_SYSTEM_PROCESSOR})
set(OCEAN_TARGET_PROCESSOR_TYPE ${CMAKE_SYSTEM_PROCESSOR})

message(CHECK_START "Third-party dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/../third-party_cache")
include(CPM.cmake)

set(CMAKE_PLATFORM_NO_VERSIONED_SONAME TRUE)

# Do not change the order of these.
if(BUILD_PASS_INDEX EQUAL 0)
    include(libjpeg.cmake)
    include(gl.cmake)
    include(zlib.cmake)
    include(eigen.cmake)
    include(giflib.cmake)
    include(openxr.cmake)
    include(tinyxml2.cmake)
    include(protozero.cmake)
    include(harfbuzz.cmake)

    if (ANDROID OR LINUX)
        include(mbedtls.cmake)
    endif()

    if (ANDROID)
        add_subdirectory(arcore-android-sdk)
    endif()

elseif(BUILD_PASS_INDEX EQUAL 1)

    include(libpng.cmake)
    include(libtiff.cmake)
    include(vtzero.cmake)
    if (ANDROID OR LINUX)
        include(curl.cmake)
    endif()
    include(libyuv.cmake)

elseif(BUILD_PASS_INDEX EQUAL 2)

    include(freetype.cmake)
    include(assimp.cmake)

    if (MACOS OR LINUX OR WIN32)
        include(wxwidgets.cmake)
    endif()

    # Disabled by default
    # include(opencv.cmake)
endif()

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
