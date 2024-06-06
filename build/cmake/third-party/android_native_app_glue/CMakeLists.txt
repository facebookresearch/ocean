# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree

cmake_minimum_required(VERSION 3.26)

set(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")
set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install path for headers")

find_library(ANDROID_LIB     NAMES android)
find_library(ANDROID_LOG_LIB NAMES log)

add_library(android_native_app_glue STATIC "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c")

target_compile_definitions(android_native_app_glue PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
target_compile_options(android_native_app_glue PUBLIC ${OCEAN_COMPILER_FLAGS} "-fexceptions")

target_link_libraries(android_native_app_glue
    PUBLIC
        ${ANDROID_LIB}
    PRIVATE
        ${ANDROID_LOG_LIB}
)

install(TARGETS android_native_app_glue
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(FILES "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.h"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    COMPONENT include
)
