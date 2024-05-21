# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(jni)

if (ANDROID OR IOS OR LINUX OR MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_network")

    if (ANDROID OR LINUX)
        find_package(CURL QUIET)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (IOS OR MACOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    if ((ANDROID OR LINUX) AND NOT CURL_FOUND)
        message(STATUS "Excluding 'ocean/network/HTTPSClient.{cpp, h}' because Curl was not found.")
        list(REMOVE_ITEM OCEAN_TARGET_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/HTTPSClient.h)
        list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/HTTPSClient.cpp)
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_NETWORK_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_io
    )

    if ((ANDROID OR LINUX) AND CURL_FOUND)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PRIVATE
                CURL::libcurl
        )
    endif()

    if (WIN32)
        target_link_libraries(${OCEAN_TARGET_NAME}
            PRIVATE
                Winhttp
                Ws2_32
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/network
            COMPONENT include
    )

endif()
