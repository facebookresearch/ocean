# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_test_testmedia")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (MACOS OR IOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    # Exclude the following source files until VRS support becomes available
    list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/TestVRS.h"
    )

    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/TestVRS.cpp"
    )

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            "${OCEAN_PREPROCESSOR_FLAGS}"
            "-DOCEAN_DO_NOT_TEST_VRS"
    )

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_TEST_MEDIA_EXPORT")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_COMPILER_FLAGS}")

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME}
            PRIVATE
                "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_media
            ocean_media_ffmpeg
            ocean_media_openimagelibraries
            ocean_media_special
            # ocean_media_vrs  # Not yet available.
            ocean_system
            ocean_test
    )

    if (ANDROID)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_android
                ocean_platform_android
        )
    endif()

    if (MACOS OR IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_media_avfoundation
            ocean_media_imageio
        )
    endif()

    if (WIN32)
        target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_media_wic
            ocean_media_mediafoundation
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/test/testmedia
            COMPONENT include
    )

endif()

if (ANDROID OR IOS OR LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_test_testmedia_gtest")

    find_package(GTest REQUIRED)

    enable_testing()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    if (MACOS OR IOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()
    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TestMedia.cpp")
    if (ANDROID)
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TestMovie.cpp")
    endif()

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            "${OCEAN_PREPROCESSOR_FLAGS}"
            "-DOCEAN_USE_GTEST"
            "-DOCEAN_MEDIA_OIL_SUPPORT_GIF"
            "-DOCEAN_MEDIA_OIL_SUPPORT_JPG"
            "-DOCEAN_MEDIA_OIL_SUPPORT_PNG"
            "-DOCEAN_MEDIA_OIL_SUPPORT_TIF"
            # "-DOCEAN_MEDIA_OIL_SUPPORT_WEBP"  # Not yet available
        )

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_TEST_MEDIA_EXPORT")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_COMPILER_FLAGS}")

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PRIVATE "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            GTest::gtest_main
            ocean_base
            ocean_cv
            ocean_media
            ocean_media_ffmpeg
            ocean_media_openimagelibraries
            ocean_media_special
            ocean_system
            ocean_test
    )

    if (MACOS OR IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_media_avfoundation
            ocean_media_imageio
        )
    endif()

    if (WIN32)
        target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_media_wic
            ocean_media_mediafoundation
        )
    endif()

    include(GoogleTest)
    gtest_add_tests(TARGET ${OCEAN_TARGET_NAME} WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

endif()
