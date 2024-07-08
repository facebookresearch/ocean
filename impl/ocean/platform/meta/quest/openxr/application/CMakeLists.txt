# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (ANDROID)

    set(APPLICATION_ADVANCED_HEADER_FILE "${CMAKE_CURRENT_LIST_DIR}/VRNativeApplicationAdvanced.h")
    set(APPLICATION_ADVANCED_SOURCE_FILE "${CMAKE_CURRENT_LIST_DIR}/VRNativeApplicationAdvanced.cpp")

    set(OCEAN_TARGET_NAME "ocean_platform_meta_quest_openxr_application")

    find_package(AndroidNativeAppGlue REQUIRED)
    find_package(OpenXR REQUIRED)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES "${APPLICATION_ADVANCED_HEADER_FILE}")
    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${APPLICATION_ADVANCED_SOURCE_FILE}")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} STATIC ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS} "-DOCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR")

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_math
            ocean_platform_gles
            ocean_platform_meta_quest
            ocean_platform_meta_quest_application
            ocean_platform_meta_quest_openxr
            ocean_platform_openxr
            ocean_rendering
            ocean_rendering_glescenegraph_quest_openxr
            AndroidNativeAppGlue::AndroidNativeAppGlue
            OpenXR::openxr_loader
        PRIVATE
            ocean_base
            ocean_cv_fonts
            ocean_platform_android
            ocean_platform_linux
            ocean_rendering_glescenegraph
    )

    # Prevent 'ANativeActivity_onCreate()' from being stripped out as an unused sympol by linker.  It is called by Android's native activity framework.
    target_link_options(${OCEAN_TARGET_NAME} PUBLIC  "LINKER:--undefined=ANativeActivity_onCreate")

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/meta/quest/openxr/application
            COMPONENT include
    )

    if (BUILD_SHARED_LIBS)
        # Avoid building "ocean_platform_meta_quest_openxr_application_advanced" when performing shared library build.
        # Requisite library registration functions (e.g. `SceneDescription::SDX::X3D::registerX3DLibrary()`) are missing
        # in shared library builds of dependencies.

        message(STATUS "Excluding 'ocean_platform_meta_quest_openxr_application_advanced' for shared library build.")
        return()
    endif()

    set(OCEAN_TARGET_NAME "ocean_platform_meta_quest_openxr_application_advanced")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} STATIC ${APPLICATION_ADVANCED_SOURCE_FILE} ${APPLICATION_ADVANCED_HEADER_FILE})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_platform_meta_quest_openxr
            ocean_platform_meta_quest_openxr_application
            ocean_rendering
            ocean_scenedescription
        PRIVATE
            ocean_io
            ocean_media_android
            ocean_media_openimagelibraries
            ocean_platform_android
            ocean_scenedescription_sdl_assimp
            ocean_scenedescription_sdl_obj
            ocean_scenedescription_sdx_x3d
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/meta/quest/openxr/application
            COMPONENT include
    )

endif()
