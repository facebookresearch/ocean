# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "application_ocean_demo_base_console")

    if (MACOS)
        find_library(COREGRAPHICS_LIB CoreGraphics)
        find_library(FOUNDATION_LIB Foundation)
        find_library(IMAGEIO_LIB ImageIO)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ocean_base)

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${COREGRAPHICS_LIB}
                ${FOUNDATION_LIB}
                ${IMAGEIO_LIB}
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_demo_base_console_android_native")

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

    # The output name of this target must match the one used in `android/DemoConsoleActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanDemoBaseConsole)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_math
            ocean_platform_android
            "-Wl,--whole-archive"
            ocean_base_jni
            "-Wl,--no-whole-archive"
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/base/console/android
            COMPONENT include
    )

endif()

if (IOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_base_console_ios")

    find_library(UIKIT_LIB UiKit)

    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/ios/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/ios/*.mm")

    file(GLOB OCEAN_TARGET_STORYBOARDS "${CMAKE_SOURCE_DIR}/res/application/ocean/demo/base/console/ios/*.storyboard")

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

    set_xcode_property(${OCEAN_TARGET_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.app.demo.base.console.ios" All)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_platform_apple
            ocean_platform_apple_ios
            ${UIKIT_LIB}
    )

endif()
