# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (APPLE OR
    ANDROID OR
    IOS OR
    LINUX OR
    WIN32)

    if (APPLE)
        find_library(COREGRAPHIS_LIB CoreGraphics)
        find_library(FOUNDATION_LIB Foundation)
        find_library(IMAGEIO_LIB ImageIO)
    elseif (IOS)
        find_library(FOUNDATION_LIB Foundation)
        find_library(UIKIT_LIB UiKit)
    endif()

    set(OCEAN_TARGET_NAME "ocean_test_testbase")

    project(${OCEAN_TARGET_NAME})

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PUBLIC
            "${OCEAN_PREPROCESSOR_FLAGS}"
    )

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME}
            PUBLIC
                "${OCEAN_COMPILER_FLAGS}"

            PRIVATE
                "-fexceptions"
        )
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_system
    )

    if (APPLE)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${FOUNDATION_LIB})
    elseif(IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${FOUNDATION_LIB}
                ${UIKIT_LIB}
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${OCEAN_LIBRARY_INSTALL_DIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/base
            COMPONENT include
    )

endif()
