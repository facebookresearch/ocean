# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "application_ocean_test_platform_testplatform")

    if (MACOS)
        find_library(FOUNDATION_LIB Foundation)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_system
            ocean_test_testplatform
    )

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${FOUNDATION_LIB})
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()

if (IOS)

    set(OCEAN_TARGET_NAME "application_ocean_test_platform_testplatform_ios")

    find_library(UIKIT_LIB UiKit)

    file(GLOB OCEAN_TARGET_HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.h"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.h"
    )
    file(GLOB OCEAN_TARGET_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.mm"
    )

    file(GLOB OCEAN_TARGET_STORYBOARDS "${CMAKE_SOURCE_DIR}/res/application/ocean/test/platform/testplatform/ios/*.storyboard")

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

    set_xcode_property(${OCEAN_TARGET_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.app.test.platform.testplatform.ios" All)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_system
            ocean_test_testplatform
            ${UIKIT_LIB}
    )
endif()
