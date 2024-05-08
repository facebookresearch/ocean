# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (ANDROID OR IOS OR MACOS OR WINDOWS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_messengercode")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_cv
            ocean_cv_detector
            ocean_math
            ocean_media
            ocean_platform
            ocean_rendering
    )

    if (MACOS OR IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_avfoundation
                ocean_media_imageio
        )
    endif()

    if (ANDROID)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_openimagelibraries
        )
    endif()

    if (WINDOWS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_directshow
                ocean_media_mediafoundation
                ocean_media_wic
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/cv/detector/messengercode
        COMPONENT include
    )

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_messengercode_android_native")

    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.cpp")

    add_library(${OCEAN_TARGET_NAME} SHARED ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS} "-DOCEAN_RUNTIME_SHARED")
    target_compile_options(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_COMPILER_FLAGS}

        PRIVATE
            "-fexceptions"
    )

    # The output name of this target must match the one used in `android/MessengerCodeActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanDemoCVDetectorMessengerCode)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            application_ocean_demo_cv_detector_messengercode
            ocean_media_android
            ocean_platform_android
            ocean_rendering_glescenegraph_android
            "-Wl,--whole-archive"
            ocean_base_jni
            ocean_media_android_jni
            ocean_platform_android_application_jni
            ocean_rendering_glescenegraph_jni
            "-Wl,--no-whole-archive"
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/cv/detector/messengercode/android
        COMPONENT include
    )

endif()
