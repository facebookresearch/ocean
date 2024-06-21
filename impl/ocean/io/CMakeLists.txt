# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(image)
add_subdirectory(maps)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_io")

    find_package(ZLIB REQUIRED)
    find_package(tinyxml2 QUIET)

    if (NOT IOS)
        # CMake accepts system-level SSL instances which were built for macOS. This causes iOS builds to fail.
        # To avoid this problem, SSL won't be used on iOS for now.
        find_package(OpenSSL QUIET)
    endif()

    if (MACOS)
        find_library(FOUNDATION_LIB Foundation)
        find_library(SECURITY_LIB Security)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (MACOS OR IOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    if (NOT OpenSSL_FOUND)
        message(STATUS "Excluding 'ocean/io/Signature.{cpp, h, mm}' because OpenSSL hasn't been found.")
        list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/Signature.h")
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Signature.cpp")
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Signature.mm")
    endif()

    if (NOT tinyxml2_FOUND)
        message(STATUS "Excluding 'ocean/io/XMLConfig.{cpp, h}' because tinyxml2 hasn't been found.")
        list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/XMLConfig.h")
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/XMLConfig.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_IO_EXPORT")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_math
            ZLIB::ZLIB
    )

    if (OpenSSL_FOUND)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    endif()

    if (tinyxml2_FOUND)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC tinyxml2::tinyxml2)
    endif()

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${FOUNDATION_LIB} ${SECURITY_LIB})
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/io
            COMPONENT include
    )

endif()
