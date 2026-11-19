# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

add_subdirectory(testadvanced)
add_subdirectory(testdetector)
add_subdirectory(testsegmentation)
add_subdirectory(testsynthesis)

if (MACOS OR LINUX OR WIN32 OR EMSCRIPTEN)

    set(OCEAN_TARGET_NAME "application_ocean_test_cv_testcv")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})

    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME} PRIVATE "-fexceptions")
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_math
            ocean_system
            ocean_test_testcv
    )

    if (EMSCRIPTEN)
        # Put the browser shell beside the generated .js/.wasm so that serving the
        # output directory is enough to run the tests in a browser. The android/ and
        # ios/ shells are likewise consumed by their own builds rather than copied by
        # hand.
        #
        # configure_file() rather than a POST_BUILD custom command: the shell is not an
        # input to the executable, so editing it does not make the target out of date
        # and a POST_BUILD step would simply never run again. configure_file() makes
        # CMake track the file and re-copy whenever it changes.
        #
        # The destination assumes the executable lands in CMAKE_CURRENT_BINARY_DIR,
        # which holds unless a consumer sets CMAKE_RUNTIME_OUTPUT_DIRECTORY. Ocean
        # only does that for WIN32. $<TARGET_FILE_DIR:> would be robust against it but
        # needs file(GENERATE), which is more machinery than a copied shell warrants.
        configure_file(
            "${CMAKE_CURRENT_LIST_DIR}/wasm/index.html"
            "${CMAKE_CURRENT_BINARY_DIR}/index.html"
            COPYONLY
        )

        # install(TARGETS) installs only what CMake considers the target's file, and
        # the Emscripten toolchain sets CMAKE_EXECUTABLE_SUFFIX to ".js". The .wasm
        # beside it is a side file CMake knows nothing about, so without this the
        # install tree holds a loader that aborts with ENOENT on the module it loads.
        install(FILES
                "$<TARGET_FILE_DIR:${OCEAN_TARGET_NAME}>/$<TARGET_FILE_BASE_NAME:${OCEAN_TARGET_NAME}>.wasm"
            DESTINATION bin
        )

        # The shell goes with it, or the installed tree cannot be opened in a browser.
        # Renamed on the way in: every test application installs into the same bin/,
        # so shells kept as index.html would silently overwrite one another and leave
        # a single page pointing at whichever target CMake happened to install last.
        install(FILES "${CMAKE_CURRENT_LIST_DIR}/wasm/index.html"
            DESTINATION bin
            RENAME "${OCEAN_TARGET_NAME}.html"
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()

if (ANDROID)

    set(OCEAN_TARGET_NAME "application_ocean_test_cv_testcv_android_native")

    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/android/*.cpp")

    add_library(${OCEAN_TARGET_NAME} SHARED ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME}
        PUBLIC
            ${OCEAN_COMPILER_FLAGS}

        PRIVATE
            "-fexceptions"
    )

    # The output name of this target must match the one used in `android/TestCVActivity.java`
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES OUTPUT_NAME OceanTestCV)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            "-Wl,--whole-archive"
            ocean_base_jni
            "-Wl,--no-whole-archive"
        PRIVATE
            ocean_platform_android
            ocean_test_testcv
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/test/cv/testcv/android
            COMPONENT include
    )

endif()

if (IOS)

    set(OCEAN_TARGET_NAME "application_ocean_test_cv_testcv_ios")

    find_library(UIKIT_LIB UiKit)

    file(GLOB OCEAN_TARGET_HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.h"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.h"
    )
    file(GLOB OCEAN_TARGET_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ios/*.mm"
    )

    # Exclude files that are intended for the Desktop version of this application.
    list(REMOVE_ITEM OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/TestCV.cpp")

    file(GLOB OCEAN_TARGET_STORYBOARDS "${CMAKE_SOURCE_DIR}/res/application/ocean/test/cv/testcv/ios/*.storyboard")

    add_executable(${OCEAN_TARGET_NAME}
        ${OCEAN_TARGET_HEADER_FILES}
        ${OCEAN_TARGET_SOURCE_FILES}
        ${OCEAN_TARGET_STORYBOARDS}
    )

    set_source_files_properties(${OCEAN_TARGET_STORYBOARDS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(ALL_APP_RESOURCES
        ${OCEAN_TARGET_STORYBOARDS}
    )

    set_target_properties(${OCEAN_TARGET_NAME}
        PROPERTIES
            RESOURCE "${ALL_APP_RESOURCES}"
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/ios/Info.plist"
    )

    set_xcode_property(${OCEAN_TARGET_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.app.test.cv.testcv.ios" All)

    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_test_testcv
            ${UIKIT_LIB}
    )
endif()
