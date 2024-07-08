# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (LINUX OR MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_misc_appinspector_binaryanalyzer")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_io
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/application/ocean/demo/misc/appinspector
        COMPONENT include
    )

endif()

if (MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_misc_appinspector_osx")

    find_library(APPKIT_LIB AppKit)

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/osx/*.mm")

    # Target definition
    add_executable(${OCEAN_TARGET_NAME} MACOSX_BUNDLE ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    set(MACOSX_DEPLOYMENT_TARGET "10.12")
    set(PRODUCT_BUNDLE_IDENTIFIER "com.meta.ocean.application.demo.misc.appinspector.osx")
    set(EXECUTABLE_NAME ${OCEAN_TARGET_NAME})
    set(PRODUCT_NAME ${OCEAN_TARGET_NAME})
    configure_file(${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${OCEAN_TARGET_NAME} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    )

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_IMPL_DIR})

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS} -fobjc-arc)

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PRIVATE
            application_ocean_demo_misc_appinspector_binaryanalyzer
            ocean_base
            ocean_cv
            ocean_io
            ocean_math
            ocean_platform_apple
            ocean_platform_apple_macos
            ocean_system

            ${APPKIT_LIB}
    )

    # Custom command to compile Application.xib to Application.nib
    set(APPLICATION_XIB "${OCEAN_IMPL_DIR}/../res/application/ocean/demo/misc/appinspector/osx/Application.xib")
    add_custom_command(TARGET ${OCEAN_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Resources
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" "${CMAKE_COMMAND}" -E remove ${CMAKE_CURRENT_BINARY_DIR}/Resources/Application.nib
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" "${CMAKE_COMMAND}" -E echo "Compiling XIB file..."
        COMMAND ${CMAKE_COMMAND} -E env "IBTOOL=${CMAKE_PROGRAM_PATH}" ibtool --warnings --notices --errors --output-format human-readable-text --compile ${CMAKE_CURRENT_BINARY_DIR}/Resources/Application.nib "${APPLICATION_XIB}"
        COMMENT "Compiling XIB file..."
        VERBATIM
    )

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} BUNDLE DESTINATION bin)

    # Install Application.nib
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources DESTINATION bin/${OCEAN_TARGET_NAME}.app/Contents)

    # Install needed dependencies in app bundle.  Overwrite app's code signature as workaround for invalid signature error.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\" \"\" \"${OCEAN_MACOSX_BUNDLE_LIBRARY_PATHS}\")
        execute_process(COMMAND codesign -s - -f --deep \"${CMAKE_INSTALL_PREFIX}/bin/${OCEAN_TARGET_NAME}.app\")
        " COMPONENT Runtime)

endif()

if (LINUX OR MACOS)

    set(OCEAN_TARGET_NAME "application_ocean_demo_misc_appinspector_cmd")

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/linux/*.h")
    file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/linux/*.cpp")

    add_executable(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    target_include_directories(${OCEAN_TARGET_NAME} PRIVATE "${OCEAN_IMPL_DIR}")

    target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_PREPROCESSOR_FLAGS})
    target_compile_options(${OCEAN_TARGET_NAME} PRIVATE ${OCEAN_COMPILER_FLAGS} "-fexceptions")

    target_link_libraries(${OCEAN_TARGET_NAME}
        PRIVATE
            application_ocean_demo_misc_appinspector_binaryanalyzer
            ocean_base
    )

    if (BUILD_SHARED_LIBS)
        set_property(TARGET ${OCEAN_TARGET_NAME} APPEND PROPERTY INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME} DESTINATION bin)

endif()
