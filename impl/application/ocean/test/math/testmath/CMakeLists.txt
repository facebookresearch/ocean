# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (APPLE OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "application_ocean_test_math_testmath")

    if (APPLE)
        find_library(FOUNDATION_LIB Foundation)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_MATH_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    if (BUILD_SHARED_LIBS)
        if (APPLE OR LINUX)
            set_property(TARGET ${OCEAN_TARGET_NAME} APPEND PROPERTY INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${OCEAN_LIBRARY_INSTALL_DIR}")
        endif()
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_system
            ocean_math
            ocean_test_testmath
    )

    if (APPLE)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
                ${FOUNDATION_LIB}
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()
