# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS)

    find_package(OpenCV QUIET COMPONENTS core imgproc)

    if (NOT OpenCV_FOUND)
        message(STATUS "Excluding 'application_ocean_demo_misc_imageannotator_osx' because OpenCV hasn't been found.")
        return()
    endif()

    find_package(Eigen3 REQUIRED) # Workaround linker error

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
            ${OpenCV_INCLUDE_DIRS}
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
            ocean_cv_detector
            ocean_io
            ocean_math
            ocean_media
            ocean_media_imageio
            ocean_platform_wxwidgets
            ${OpenCV_LIBS}
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} BUNDLE DESTINATION bin)

    # Install needed dependencies in app bundle.  Overwrite app's code signature as workaround for invalid signature error.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\" \"\" \"${OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS}\")
        execute_process(COMMAND codesign -s - -f --deep \"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\")
        " COMPONENT Runtime)

endif()
