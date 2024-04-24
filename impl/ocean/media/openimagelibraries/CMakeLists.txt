# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_media_openimagelibraries")

    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)

    set(PREPROCESSOR_DEFINES -DOCEAN_MEDIA_OIL_SUPPORT_PNG -DOCEAN_MEDIA_OIL_SUPPORT_JPG)

    set(IMAGE_LIBRARIES
        ${PNG_LIBRARIES}
        ${JPEG_LIBRARIES}
    )

    if (LINUX)
        find_package(GIF REQUIRED)
        list(APPEND PREPROCESSOR_DEFINES -DOCEAN_MEDIA_OIL_SUPPORT_GIF)
        list(APPEND IMAGE_LIBRARIES ${GIF_LIBRARIES})
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
    file(GLOB OCEAN_TARGET_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

    if (BUILD_SHARED_LIBS)
        list(APPEND PREPROCESSOR_DEFINES -DUSE_OCEAN_MEDIA_OIL_EXPORT)
    endif()

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${PREPROCESSOR_DEFINES})

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
            ${IMAGE_LIBRARIES}
    )

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
