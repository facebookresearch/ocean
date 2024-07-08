# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(jni)

if (ANDROID OR IOS OR LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_scenedescription_sdx_x3d")

    find_package(tinyxml2 QUIET)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    if (tinyxml2_FOUND)
        set(OCEAN_ENABLE_TINYXML2_PREPROCESSOR_FLAG "-DOCEAN_HAS_TINYXML2")
    else()
        message(STATUS "Excluding 'ocean/scenedescription/sdx/x3d/XMLParser.{cpp, h}' because tinyxml2 hasn't been found.")
        list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/XMLParser.h")
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/XMLParser.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_PREPROCESSOR_FLAGS}

        PRIVATE
            ${OCEAN_ENABLE_TINYXML2_PREPROCESSOR_FLAG}
    )

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_devices
            ocean_io
            ocean_math
            ocean_media
            ocean_rendering
            ocean_scenedescription
    )

    if (tinyxml2_FOUND)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC tinyxml2::tinyxml2)
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/scenedescription/sdx/x3d
        COMPONENT include
    )

endif()
