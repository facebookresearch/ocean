# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR ANDROID OR LINUX)

    set(OCEAN_TARGET_NAME "application_ocean_test_cv_testcv_testdetector_testqrcodes")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})

    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PRIVATE "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_cv_detector
            ocean_cv_detector_qrcodes
            ocean_io
            ocean_math
            ocean_media
            ocean_media_openimagelibraries
            ocean_system
            ocean_test_testcv
            ocean_test_testcv_testdetector
            ocean_test_testcv_testdetector_testqrcodes
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_test_cv_testcv_testdetector_testqrcodes_android_native")

    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.cpp")

    add_library(${OCEAN_TARGET_NAME} SHARED ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_COMPILER_FLAGS}

        PRIVATE
            "-fexceptions"
    )

    # The output name of this target must match the one used in `android/TestCVDetectorQRCodesActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanTestCVDetectorQRCodes)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_base_jni
            ocean_math
            ocean_platform_android
            ocean_test_testcv_testdetector_testqrcodes
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/test/cv/testcv/testdector/testqrcodes/android
            COMPONENT include
    )

endif()
