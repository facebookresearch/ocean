# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(apple)
add_subdirectory(linux)

if (APPLE OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_platform")

    if (APPLE)
        find_library(APPKIT_LIB AppKit)
        find_library(CARBON_LIB Carbon)
    elseif (IOS)
        find_library(UIKIT_LIB UiKit)
    elseif (LINUX)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (APPLE OR IOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    if (NOT APPLE)
        # Remove macOS-only files
        list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES
            "Keyboard.h"
            "Mouse.h"
        )

        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES
            "Keyboard.cpp"
            "Keyboard.mm"
            "Mouse.cpp"
            "Mouse.mm"
        )
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_PLATFORM_EXPORT)
    endif()
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
    )

    if (APPLE)
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
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${OCEAN_LIBRARY_INSTALL_DIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform
            COMPONENT include
    )

endif()
