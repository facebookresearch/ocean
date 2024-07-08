# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(jni)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_media_openimagelibraries")

    # find_package(GIF REQUIRED)  # Blocking windows build
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(TIFF REQUIRED)
    #find_package(WEBP REQUIRED)  # Not yet available

    if (LINUX)
        # Required by libtiff
        find_package(LibLZMA REQUIRED)
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_IMPL_DIR})

    if (LINUX)
        target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${LIBLZMA_INCLUDE_DIRS})
    endif()

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

    if (BUILD_SHARED_LIBS)
        list(APPEND PREPROCESSOR_DEFINES -DUSE_OCEAN_MEDIA_OIL_EXPORT)
    endif()

    target_compile_definitions(${OCEAN_TARGET_NAME}
        PRIVATE
            ${PREPROCESSOR_DEFINES}
        PUBLIC
            "-DOCEAN_MEDIA_OIL_SUPPORT_PNG"
            "-DOCEAN_MEDIA_OIL_SUPPORT_JPG"
            # "-DOCEAN_MEDIA_OIL_SUPPORT_GIF"  # Blocking windows build
            "-DOCEAN_MEDIA_OIL_SUPPORT_TIF"
            # "-DOCEAN_MEDIA_OIL_SUPPORT_WEBP"  # Not yet available
    )

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_io
            ocean_math
            ocean_media
            ocean_media_special
            # GIF::GIF  # Blocking windows build
            PNG::PNG
            JPEG::JPEG
            TIFF::TIFF
            # WEBP::WEBP  # Not yet available
    )

    if (LINUX)
        target_link_libraries(${OCEAN_TARGET_NAME} PUBLIC ${LIBLZMA_LIBRARIES})
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/media/openimagelibraries
            COMPONENT include
    )

endif()
