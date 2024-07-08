# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(testoculustags)

if (APPLE OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_test_testtracking")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            "${OCEAN_PREPROCESSOR_FLAGS}"
    )

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_TEST_TRACKING_EXPORT")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_COMPILER_FLAGS}")

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PRIVATE "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_math
            ocean_test
            ocean_tracking
        PRIVATE
            ocean_cv
            ocean_cv_detector
            ocean_geometry
            ocean_media
            ocean_system
            ocean_tracking_point
    )

    if (ANDROID)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PRIVATE
                ocean_media_android
                ocean_media_openimagelibraries
                ocean_platform_android
                ocean_platform_linux
                ocean_tracking_pattern
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/test/testtracking
            COMPONENT include
    )

endif()

if (ANDROID OR IOS OR LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_test_testtracking_gtest")

    find_package(GTest REQUIRED)

    enable_testing()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TestTracking.cpp")
    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TestPatternTracker.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            "${OCEAN_PREPROCESSOR_FLAGS}"
            "-DOCEAN_USE_GTEST"
    )

    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_TEST_TRACKING_EXPORT")
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
        PRIVATE
            ocean_cv
            ocean_math
            ocean_system
            ocean_tracking_point
    )

    include(GoogleTest)
    gtest_add_tests(TARGET ${OCEAN_TARGET_NAME} WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

endif()
