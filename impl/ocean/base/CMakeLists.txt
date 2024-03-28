# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (APPLE OR
    ANDROID OR
    IOS OR
    LINUX OR
    WIN32)

    if (APPLE)
        find_library(FOUNDATION_LIB Foundation)
    elseif (IOS)
        find_library(FOUNDATION_LIB Foundation)
        find_library(UIKIT_LIB UiKit)
    elseif (LINUX)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
    endif()

    set(OCEAN_TARGET_NAME "ocean_base")

    project(${OCEAN_TARGET_NAME})

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

    if (APPLE)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    if (APPLE)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${FOUNDATION_LIB})
    elseif(IOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${FOUNDATION_LIB}
                ${UIKIT_LIB}
        )
    elseif(LINUX)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${CMAKE_DL_LIBS}
                Threads::Threads
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
