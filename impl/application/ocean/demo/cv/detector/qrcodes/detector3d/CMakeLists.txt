# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

set(TARGET_NAME_COMMON_FRAGMENT "application_ocean_demo_cv_detector_qrcodes_detector3d")
set(APP_LIBRARY_TARGET_NAME ${TARGET_NAME_COMMON_FRAGMENT})
set(DIR_STRUCTURE_COMMON_FRAGMENT "application/ocean/demo/cv/detector/qrcodes/detector3d")

if (MACOS)

    set(OCEAN_TARGET_NAME "${APP_LIBRARY_TARGET_NAME}")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_devices
            ocean_math
            ocean_media
        PRIVATE
            ocean_cv
            ocean_cv_detector_qrcodes
            ocean_io
            ocean_platform
    )

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_media_avfoundation
                ocean_media_imageio
        )
    endif()

    if (WIN32)
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
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${DIR_STRUCTURE_COMMON_FRAGMENT}
        COMPONENT include
    )

endif()


if (MACOS)

    find_program (IBTOOL NAMES ibtool REQUIRED)
    find_program (ACTOOL NAMES actool REQUIRED)

    set(OCEAN_TARGET_NAME "${TARGET_NAME_COMMON_FRAGMENT}_osx")

    set(APP_RESOURCES_SRC_DIR "${OCEAN_IMPL_DIR}/../res/${DIR_STRUCTURE_COMMON_FRAGMENT}/osx")
    set(APP_RESOURCES_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/osx/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/osx/*.mm")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    set(MACOSX_DEPLOYMENT_TARGET "10.12")
    set(PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.application.demo.cv.detector.barcodes.detector2d.osx")
    set(EXECUTABLE_NAME ${OCEAN_TARGET_NAME})
    set(PRODUCT_NAME ${OCEAN_TARGET_NAME})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    )

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PRIVATE
            ${APP_LIBRARY_TARGET_NAME}
            ocean_base
            ocean_platform_apple
            ocean_platform_apple_macos
    )

    # Custom command to compile Application.xib to Application.nib
    set(APP_NIB_TARGET_NAME "${OCEAN_TARGET_NAME}_nib")
    set(APPLICATION_XIB "${APP_RESOURCES_SRC_DIR}/Application.xib")
    set(APPLICATION_NIB "${APP_RESOURCES_OUT_DIR}/Application.nib")
    add_custom_command(
        OUTPUT  ${APPLICATION_NIB}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_RESOURCES_OUT_DIR}
        COMMAND ${IBTOOL} --warnings --notices --errors --output-format human-readable-text --compile "${APPLICATION_NIB}" "${APPLICATION_XIB}"
        DEPENDS ${APPLICATION_XIB}
        VERBATIM
    )
    add_custom_target(${APP_NIB_TARGET_NAME} DEPENDS ${APPLICATION_NIB})
    add_dependencies(${OCEAN_TARGET_NAME} ${APP_NIB_TARGET_NAME})

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

    # Install Resources files
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources DESTINATION bin/${OCEAN_TARGET_NAME}.app/Contents)

    # Install needed dependencies in app bundle.  Overwrite app's code signature as workaround for invalid signature error.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\" \"\" \"${OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS}\")
        execute_process(COMMAND codesign -s - -f --deep \"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\")
        " COMPONENT Runtime)

endif()
