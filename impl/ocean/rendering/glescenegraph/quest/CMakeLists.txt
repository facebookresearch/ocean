# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (ANDROID)

    set(OCEAN_TARGET_NAME "ocean_rendering_glescenegraph_quest_openxr")

    # Source files
    file(GLOB QUEST_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB QUEST_OPENXR_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/openxr/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/openxr/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${QUEST_HEADER_FILES} ${QUEST_OPENXR_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS} "-DOCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR")

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_platform_meta_quest_openxr
            ocean_rendering
            ocean_rendering_glescenegraph
        PRIVATE
            ocean_platform_openxr
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${QUEST_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/rendering/glescenegraph/quest
            COMPONENT include
    )

    install(FILES ${QUEST_OPENXR_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/rendering/glescenegraph/quest/openxr
            COMPONENT include
    )

endif()
