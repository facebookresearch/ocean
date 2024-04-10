# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (APPLE OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "application_ocean_test_io_testio")

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
    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_math
            ocean_system
            ocean_test_testio
    )

    if (APPLE)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${FOUNDATION_LIB})
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()
