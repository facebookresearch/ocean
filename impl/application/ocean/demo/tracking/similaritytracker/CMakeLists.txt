# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (ANDROID OR IOS OR MACOS OR WINDOWS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_tracking_similaritytracker")

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
            ocean_base
            ocean_cv
            ocean_devices
            ocean_geometry
            ocean_io
            ocean_math
            ocean_media
            ocean_platform
            ocean_tracking
            ocean_tracking_point
    )

    if (MACOS OR IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_avfoundation
                ocean_media_imageio
        )
    endif()

    if (IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_devices_ios
        )
    endif()

    if (ANDROID)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_devices_android
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
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/tracking/similaritytracker
        COMPONENT include
    )

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_demo_tracking_similaritytracker_android_native")

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

    # The output name of this target must match the one used in `android/SimilarityTrackerActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanDemoTrackingSimilarityTracker)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            application_ocean_demo_tracking_similaritytracker
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
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/tracking/similaritytracker/android
        COMPONENT include
    )

endif()
