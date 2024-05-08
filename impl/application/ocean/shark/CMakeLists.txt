# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_shark_android_native")

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

    # The output name of this target must match the one used in `android/SharkActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanShark)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_devices
            ocean_devices_android
            ocean_devices_blob
            ocean_devices_pattern
            ocean_devices_slam
            ocean_math
            ocean_media
            ocean_media_android
            ocean_media_openimagelibraries
            ocean_platform
            ocean_platform_android
            ocean_rendering
            ocean_rendering_glescenegraph_android
            ocean_scenedescription
            ocean_scenedescription_sdl_obj
            ocean_scenedescription_sdx_x3d
            ocean_tracking
            ocean_tracking_blob
            ocean_tracking_orb
            ocean_tracking_pattern
            "-Wl,--whole-archive"
            ocean_base_jni
            ocean_devices_android_jni
            ocean_devices_pattern_jni
            ocean_media_android_jni
            ocean_media_openimagelibraries_jni
            ocean_platform_android_application_jni
            ocean_rendering_glescenegraph_jni
            ocean_scenedescription_sdl_obj_jni
            ocean_scenedescription_sdx_x3d_jni
            "-Wl,--no-whole-archive"
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/shark/android
        COMPONENT include
    )

endif()
