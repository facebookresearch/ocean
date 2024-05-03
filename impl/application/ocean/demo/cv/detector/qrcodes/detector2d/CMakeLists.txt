# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (ANDROID OR IOS OR MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_qrcodes_detector2d")

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
            ocean_cv_detector_qrcodes
            ocean_media
            ocean_platform
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

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/cv/detector/qrcodes/detector2d
        COMPONENT include
    )

endif()

if (MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_qrcodes_detector2d_osx")

    find_library(APPKIT_LIB AppKit)
    find_library(AVFOUNDATION_LIB AVFoundation)
    find_library(COREMEDIA_LIB CoreMedia)
    find_library(COREVIDEO_LIB CoreVideo)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.mm")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} MACOSX_BUNDLE ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    set(MACOSX_DEPLOYMENT_TARGET "10.12")
    set(PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.application.demo.cv.detector.qrcodes.detector.osx")
    set(EXECUTABLE_NAME ${OCEAN_TARGET_NAME})
    set(PRODUCT_NAME ${OCEAN_TARGET_NAME})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    )

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS} -DUSE_OCEAN_DEVICES_VRS)
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS} -fobjc-arc)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            application_ocean_demo_cv_detector_qrcodes_detector2d
            ocean_platform_apple_macos
            ${APPKIT_LIB}
            ${AVFOUNDATION_LIB}
            ${COREMEDIA_LIB}
            ${COREVIDEO_LIB}
    )

    # Custom command to compile Application.xib to Application.nib
    set(APPLICATION_XIB "${OCEAN_IMPL_DIR}/../res/application/ocean/demo/cv/detector/qrcodes/detector2d/osx/Application.xib")
    add_custom_command(TARGET ${OCEAN_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Resources
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" "${CMAKE_COMMAND}" -E remove ${CMAKE_CURRENT_BINARY_DIR}/Resources/Application.nib
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" "${CMAKE_COMMAND}" -E echo "Compiling XIB file..."
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" ibtool --warnings --notices --errors --output-format human-readable-text --compile ${CMAKE_CURRENT_BINARY_DIR}/Resources/Application.nib "${APPLICATION_XIB}"
        COMMENT "Compiling XIB file..."
        VERBATIM
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} BUNDLE DESTINATION bin)

    # Install Application.nib
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources DESTINATION bin/${OCEAN_TARGET_NAME}.app/Contents)

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_qrcodes_detector2d_android_native")

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

    # The output name of this target must match the one used in `android/Detector2DActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanDemoCVDetectorQRCodes)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            application_ocean_demo_cv_detector_qrcodes_detector2d
            ocean_devices_android
            ocean_media_android
            ocean_platform_android_application
            "-Wl,--whole-archive"
            ocean_base_jni
            ocean_devices_android_jni
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
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/cv/detector/qrcodes/detector2d/android
        COMPONENT include
    )

endif()
