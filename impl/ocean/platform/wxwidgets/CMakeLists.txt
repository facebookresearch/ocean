# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_platform_wxwidgets")

    find_package(wxWidgets REQUIRED)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (MACOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_PLATFORM_WXWIDGETS_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME}
            PRIVATE
                "-Wno-unused-variable"
                "-Wno-shadow"
                "-Wno-global-constructors"
        )
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_media
            ocean_platform
    )

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            wxWidgets::wxWidgets
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/wxwidgets
            COMPONENT include
    )

endif()
