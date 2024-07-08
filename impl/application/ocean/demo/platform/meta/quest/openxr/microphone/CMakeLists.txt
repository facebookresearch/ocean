# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (ANDROID)

    if (QUEST)

        set(OCEAN_TARGET_NAME "application_ocean_demo_platform_meta_quest_openxr_microphone_quest_native")

        if (NOT TARGET ocean_platform_meta_quest_platformsdk)
            message(WARNING "Excluding '${OCEAN_TARGET_NAME}' from build because its dependency 'ocean_platform_meta_quest_platformsdk' is missing.")
            return()
        endif()

        file(GLOB OCEAN_TARGET_HEADER_FILES "quest/*.h")
        file(GLOB OCEAN_TARGET_SOURCE_FILES "quest/*.cpp")

        add_library(${OCEAN_TARGET_NAME} SHARED ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

        target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS} "-DOCEAN_RUNTIME_SHARED")

        target_compile_options(${OCEAN_TARGET_NAME}
            PUBLIC
                ${OCEAN_COMPILER_FLAGS}
            PRIVATE
                "-fexceptions"
        )

        # The output name of this target must match the one used in `quest/MicrophoneApplicationActivity.java`
        set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanDemoPlatformMetaQuestOpenXRMicrophoneApplication)

        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ocean_base
                ocean_platform_meta_quest_openxr_application
                ocean_platform_meta_quest_platformsdk
                ocean_rendering
            PRIVATE
                ocean_math
        )

        # Installation
        install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT lib
        )

        install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/platform/meta/quest/openxr/microphone/quest
            COMPONENT include
        )

    endif()

endif()
