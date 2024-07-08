# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(ios)
add_subdirectory(macos)

if (MACOS OR IOS)

    set(OCEAN_TARGET_NAME "ocean_platform_apple")

    find_library(COREGRAPHICS_LIB CoreGraphics)

    if (MACOS)
        find_library(APPKIT_LIB AppKit)
        find_library(CARBON_LIB Carbon)
    elseif (IOS)
        find_library(UIKIT_LIB UiKit)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_media
            ocean_platform
    )

    target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${COREGRAPHICS_LIB})

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${APPKIT_LIB}
                ${CARBON_LIB}
        )
    elseif(IOS)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${UIKIT_LIB})
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/apple
            COMPONENT include
    )

endif()
