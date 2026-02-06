# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(jni)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_scenedescription_sdl_assimp")

    # For Python 3P layout, manually set up assimp since no CMake config is available
    if (OCEAN_THIRD_PARTY_LAYOUT STREQUAL "python")
        # Determine debug suffix
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_assimp_debug_suffix "d")
        else()
            set(_assimp_debug_suffix "")
        endif()

        # Build target string for library path
        if (BUILD_SHARED_LIBS)
            set(_assimp_link "shared")
        else()
            set(_assimp_link "static")
        endif()

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_assimp_config "_debug")
        else()
            set(_assimp_config "")
        endif()

        # Determine platform and arch
        if (IOS)
            set(_assimp_platform "ios")
        elseif (MACOS)
            set(_assimp_platform "macos")
        elseif (ANDROID)
            set(_assimp_platform "android")
        elseif (LINUX)
            set(_assimp_platform "linux")
        elseif (WIN32)
            set(_assimp_platform "windows")
        endif()

        if (ANDROID)
            if (ANDROID_ABI STREQUAL "arm64-v8a")
                set(_assimp_arch "arm64")
            elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
                set(_assimp_arch "armv7")
            else()
                set(_assimp_arch "x86_64")
            endif()
        elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
            set(_assimp_arch "arm64")
        else()
            set(_assimp_arch "x86_64")
        endif()

        set(_assimp_target "${_assimp_platform}_${_assimp_arch}_${_assimp_link}${_assimp_config}")
        set(_assimp_root "${OCEAN_THIRD_PARTY_ROOT}/assimp")
        set(_assimp_lib_dir "${_assimp_root}/lib/${_assimp_target}")
        set(_assimp_include_dir "${_assimp_root}/h/${_assimp_platform}")

        message(STATUS "assimp (Python layout): ${_assimp_lib_dir}")

        # Find the assimp library
        if (BUILD_SHARED_LIBS)
            if (WIN32)
                set(_assimp_lib_name "assimp${_assimp_debug_suffix}.dll")
            elseif (MACOS OR IOS)
                set(_assimp_lib_name "libassimp${_assimp_debug_suffix}.dylib")
            else()
                set(_assimp_lib_name "libassimp${_assimp_debug_suffix}.so")
            endif()
            set(_assimp_import_type SHARED)
        else()
            if (WIN32)
                set(_assimp_lib_name "assimp${_assimp_debug_suffix}.lib")
            else()
                set(_assimp_lib_name "libassimp${_assimp_debug_suffix}.a")
            endif()
            set(_assimp_import_type STATIC)
        endif()

        # Create an imported target for assimp
        add_library(assimp::assimp ${_assimp_import_type} IMPORTED)
        set_target_properties(assimp::assimp PROPERTIES
            IMPORTED_LOCATION "${_assimp_lib_dir}/${_assimp_lib_name}"
            INTERFACE_INCLUDE_DIRECTORIES "${_assimp_include_dir}"
        )

        set(assimp_FOUND TRUE)
    else()
        find_package(assimp REQUIRED)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_io
            ocean_math
            ocean_media
            ocean_rendering
            ocean_scenedescription
            ocean_system
    )

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
        assimp::assimp
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/scenedescription/sdl/assimp
            COMPONENT include
    )

endif()
