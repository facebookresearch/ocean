# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

find_package(OpenCV)

if (MACOS AND OpenCV_FOUND)

    set(OCEAN_TARGET_NAME "application_ocean_demo_misc_imageannotator_osx")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/wxw/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/wxw/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} MACOSX_BUNDLE ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME}
        PRIVATE
            "${OCEAN_IMPL_DIR}"
            "${CMAKE_SOURCE_DIR}/res"  # for res/application/ocean/demo/misc/imageannotator
    )

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_BUNDLE_NAME ${OCEAN_TARGET_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_GUI_IDENTIFIER com.meta.ocean.application.demo.misc.imageannotator.osx
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    )

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PRIVATE
            ocean_base
            ocean_cv
            ocean_cv_advanced
            ocean_cv_segmentation
            ocean_io
            ocean_media
            ocean_media_imageio
            ocean_platform_wxwidgets
    )

    target_link_libraries(${OCEAN_TARGET_NAME} PRIVATE OpenCV::OpenCV)

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} BUNDLE DESTINATION bin)

endif()
