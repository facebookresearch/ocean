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

if (MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_messengercode_osx")

    find_library(APPKIT_LIB AppKit)
    find_library(AVFOUNDATION_LIB AVFoundation)
    find_library(COREMEDIA_LIB CoreMedia)
    find_library(COREVIDEO_LIB CoreVideo)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.h" "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.mm" "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} MACOSX_BUNDLE ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    set(MACOSX_DEPLOYMENT_TARGET "10.12")
    set(PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.application.demo.cv.detector.messengercode.osx")
    set(EXECUTABLE_NAME ${OCEAN_TARGET_NAME})
    set(PRODUCT_NAME ${OCEAN_TARGET_NAME})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    )

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS} -fobjc-arc)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_cv_detector
            ocean_io
            ocean_math
            ocean_media_avfoundation
            ocean_media_imageio
            ocean_platform
            ocean_platform_apple_macos
            ocean_system
            ${APPKIT_LIB}
            ${AVFOUNDATION_LIB}
            ${COREMEDIA_LIB}
            ${COREVIDEO_LIB}
    )

    # Custom command to compile Application.xib to Application.nib
    set(APPLICATION_XIB "${OCEAN_IMPL_DIR}/../res/application/ocean/demo/cv/detector/messengercode/osx/Application.xib")
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

    # Install needed dependencies in app bundle.  Overwrite app's code signature as workaround for invalid signature error.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\" \"\" \"${OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS}\")
        execute_process(COMMAND codesign -s - -f --deep \"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\")
        " COMPONENT Runtime)

endif()

if (IOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_cv_detector_messengercode_ios")

    find_library(UIKIT_LIB UiKit)

    file(GLOB OCEAN_TARGET_HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.h"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.h"
    )
    file(GLOB OCEAN_TARGET_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.mm"
    )

    file(GLOB OCEAN_TARGET_STORYBOARDS "${CMAKE_SOURCE_DIR}/res/application/ocean/demo/cv/detector/messengercode/ios/*.xib")

    add_executable(${OCEAN_TARGET_NAME}
        ${OCEAN_TARGET_HEADER_FILES}
        ${OCEAN_TARGET_SOURCE_FILES}
        ${OCEAN_TARGET_STORYBOARDS}
    )

    set_source_files_properties(${OCEAN_TARGET_STORYBOARDS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(ALL_APP_RESOURCES
        ${OCEAN_TARGET_STORYBOARDS}
    )

    set_target_properties(${OCEAN_TARGET_NAME}
        PROPERTIES
            RESOURCE "${ALL_APP_RESOURCES}"
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/ios/Info.plist"
    )

    set_xcode_property(${OCEAN_TARGET_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.app.demo.cv.detector.messengercode.ios" All)

    target_include_directories(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_IMPL_DIR}
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
    )

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv_detector
            ocean_io
            ocean_media
            ocean_media_avfoundation
            ocean_media_imageio
            ocean_platform_apple_ios
            ocean_rendering
            ocean_rendering_glescenegraph_apple
            ${UIKIT_LIB}
    )

endif()
