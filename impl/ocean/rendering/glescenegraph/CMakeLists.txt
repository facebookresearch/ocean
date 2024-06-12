# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(android)
add_subdirectory(apple)
add_subdirectory(jni)
add_subdirectory(qt)
add_subdirectory(quest)
add_subdirectory(windows)

if (ANDROID OR IOS OR LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_rendering_glescenegraph")

    if (MACOS OR IOS)
        find_library(COREVIDEO_LIB CoreVideo)

        if (IOS)
            find_library(OPENGLES_LIB OpenGLES)
        else()
            find_library(OPENGL_LIB OpenGL)
        endif()
    endif()

    if (LINUX)
        find_package(OpenGL REQUIRED)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (MACOS OR IOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_RENDERING_GLES_EXPORT")
    endif()

    if (IOS OR MACOS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DGL_SILENCE_DEPRECATION")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_cv_fonts
            ocean_math
            ocean_media
            ocean_rendering
            ocean_system
    )

    if (MACOS OR IOS)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${COREVIDEO_LIB})

        if (IOS)
            target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${OPENGLES_LIB})
        else()
            target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${OPENGL_LIB})
        endif()
    endif()

    if (WIN32)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC OpenGL32)
    endif()

    if (ANDROID)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC "-lGLESv3")
    endif()

    if (LINUX)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC OpenGL::GL)
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/rendering/glescenegraph
            COMPONENT include
    )

endif()
