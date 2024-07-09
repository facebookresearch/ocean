# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "application_ocean_test_tracking_testtracking_testoculustags")

    if (MACOS)
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
        PRIVATE
            ocean_base
            ocean_system
            ocean_test_testtracking_testoculustags
    )

    if (MACOS)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PRIVATE
                ${FOUNDATION_LIB}
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()
