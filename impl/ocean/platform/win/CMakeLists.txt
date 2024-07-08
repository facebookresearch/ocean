# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (WIN32)

    set(OCEAN_TARGET_NAME "ocean_platform_win")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_PREPROCESSOR_FLAGS}
            "-DWIN32"
            "-D_CRT_SECURE_NO_DEPRECATE"
            "-D_UNICODE"
            "-DUNICODE"
    )

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE "-DUSE_OCEAN_PLATFORM_WIN_EXPORT")
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_io
            ocean_media
            ocean_platform
    )

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            "advapi32.lib"
            "comdlg32.lib"
            "gdi32.lib"
            "odbc32.lib"
            "odbccp32.lib"
            "ole32.lib"
            "oleaut32.lib"
            "shell32.lib"
            "uuid.lib"
            "winspool.lib"
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/win
            COMPONENT include
    )

endif()
