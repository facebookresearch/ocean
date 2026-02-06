# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_platform_wxwidgets")

    # For Python 3P layout, manually set up wxWidgets since no CMake config is available
    if (OCEAN_THIRD_PARTY_LAYOUT STREQUAL "python")
        # Determine debug suffix
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_wx_debug_suffix "d")
        else()
            set(_wx_debug_suffix "")
        endif()

        # Build target string for library path
        if (BUILD_SHARED_LIBS)
            set(_wx_link "shared")
        else()
            set(_wx_link "static")
        endif()

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_wx_config "_debug")
        else()
            set(_wx_config "")
        endif()

        if (MACOS)
            set(_wx_platform "macos")
            if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
                set(_wx_arch "arm64")
            else()
                set(_wx_arch "x86_64")
            endif()
        elseif (WIN32)
            set(_wx_platform "windows")
            set(_wx_arch "x86_64")
        endif()

        set(_wx_target "${_wx_platform}_${_wx_arch}_${_wx_link}${_wx_config}")
        set(_wx_root "${OCEAN_THIRD_PARTY_ROOT}/wxwidgets")
        set(_wx_lib_dir "${_wx_root}/lib/${_wx_target}")
        set(_wx_include_dir "${_wx_root}/h/${_wx_platform}/wx-3.3")

        message(STATUS "wxWidgets (Python layout): ${_wx_lib_dir}")

        # Determine library extension based on build type
        if (BUILD_SHARED_LIBS)
            if (MACOS)
                set(_wx_lib_ext ".dylib")
            else()
                set(_wx_lib_ext ".dll")
            endif()
        else()
            set(_wx_lib_ext ".a")
            if (WIN32)
                set(_wx_lib_ext ".lib")
            endif()
        endif()

        # Find the core wxWidgets libraries
        set(_wx_libraries)
        foreach(_wx_lib baseu core aui html propgrid ribbon richtext stc xrc)
            if (_wx_lib STREQUAL "baseu")
                set(_wx_lib_name "libwx_baseu-3.3${_wx_debug_suffix}${_wx_lib_ext}")
            elseif (_wx_lib STREQUAL "core")
                if (MACOS)
                    set(_wx_lib_name "libwx_osx_cocoau_${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
                else()
                    set(_wx_lib_name "libwx_mswu_${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
                endif()
            else()
                if (MACOS)
                    set(_wx_lib_name "libwx_osx_cocoau_${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
                else()
                    set(_wx_lib_name "libwx_mswu_${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
                endif()
            endif()

            if (EXISTS "${_wx_lib_dir}/${_wx_lib_name}")
                list(APPEND _wx_libraries "${_wx_lib_dir}/${_wx_lib_name}")
            endif()
        endforeach()

        # Add additional required libraries
        foreach(_wx_lib baseu_net baseu_xml)
            set(_wx_lib_name "libwx_${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
            if (EXISTS "${_wx_lib_dir}/${_wx_lib_name}")
                list(APPEND _wx_libraries "${_wx_lib_dir}/${_wx_lib_name}")
            endif()
        endforeach()

        # Add helper libraries (only for static builds - these are bundled in shared libs)
        if (NOT BUILD_SHARED_LIBS)
            foreach(_wx_lib wxregexu wxscintilla wxlexilla)
                set(_wx_lib_name "lib${_wx_lib}-3.3${_wx_debug_suffix}${_wx_lib_ext}")
                if (EXISTS "${_wx_lib_dir}/${_wx_lib_name}")
                    list(APPEND _wx_libraries "${_wx_lib_dir}/${_wx_lib_name}")
                endif()
            endforeach()
        endif()

        # Create an imported target for wxWidgets
        add_library(wxWidgets::wxWidgets INTERFACE IMPORTED)

        # The setup.h file is in the lib directory. It may be either:
        # 1. At lib/<target>/setup.h (original CMake install location)
        # 2. At lib/<target>/wx/setup.h (Ocean Python 3P layout)
        # For case 1, we need to create a wrapper directory with wx/setup.h symlink
        # For case 2, the lib directory can be used directly as an include path
        set(_wx_setup_wrapper_dir "${CMAKE_CURRENT_BINARY_DIR}/wx_setup_wrapper/wx")
        if (EXISTS "${_wx_lib_dir}/wx/setup.h")
            # Case 2: setup.h is already in wx/ subdirectory
            # Just add the lib dir to include path (no wrapper needed)
            set(_wx_setup_include "${_wx_lib_dir}")
        elseif (EXISTS "${_wx_lib_dir}/setup.h")
            # Case 1: setup.h is at lib root, create wrapper with symlink
            file(MAKE_DIRECTORY "${_wx_setup_wrapper_dir}")
            if (NOT EXISTS "${_wx_setup_wrapper_dir}/setup.h")
                file(CREATE_LINK "${_wx_lib_dir}/setup.h" "${_wx_setup_wrapper_dir}/setup.h" SYMBOLIC)
            endif()
            set(_wx_setup_include "${CMAKE_CURRENT_BINARY_DIR}/wx_setup_wrapper")
        else()
            message(WARNING "wxWidgets setup.h not found in ${_wx_lib_dir}")
            set(_wx_setup_include "")
        endif()

        set_target_properties(wxWidgets::wxWidgets PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_wx_include_dir};${_wx_setup_include}"
            INTERFACE_LINK_LIBRARIES "${_wx_libraries}"
        )

        if (MACOS)
            # Add macOS-specific compile definitions required by wxWidgets
            # These are normally provided by wxWidgetsConfig.cmake but we set them manually
            # since we're not using the CMake config in Python 3P layout
            set_property(TARGET wxWidgets::wxWidgets PROPERTY
                INTERFACE_COMPILE_DEFINITIONS
                "__WXOSX_COCOA__"
                "__WXOSX__"
                "__WXMAC__"
                "wxUSE_BASE=1"
            )

            # Add macOS frameworks required by wxWidgets
            set_property(TARGET wxWidgets::wxWidgets APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES
                "-framework Cocoa"
                "-framework IOKit"
                "-framework Carbon"
                "-framework CoreFoundation"
                "-framework AudioToolbox"
                "-framework System"
                "-framework OpenGL"
                "-framework WebKit"
            )
        elseif (WIN32)
            # Add Windows-specific compile definitions required by wxWidgets
            # These are normally provided by wxWidgetsConfig.cmake but we set them manually
            # since we're not using the CMake config in Python 3P layout
            set_property(TARGET wxWidgets::wxWidgets PROPERTY
                INTERFACE_COMPILE_DEFINITIONS
                "__WXMSW__"
                "WXUSINGDLL=0"
                "wxUSE_BASE=1"
                "_UNICODE"
                "UNICODE"
            )
        endif()

        set(wxWidgets_FOUND TRUE)
    else()
        # Use modern CMake config-file package (wxWidgets 3.3+)
        find_package(wxWidgets REQUIRED CONFIG)
    endif()

    if (LINUX)
        if(wxWidgets_USE_FILE)
            # Add platform-specific configs to include the correct headers, etc.
            include(${wxWidgets_USE_FILE})
        endif()
    endif()

    # Source files
    file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")

    if (MACOS)
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm")
    else()
        file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
    endif()

    # Target definition
    add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

    if(CMAKE_VERSION VERSION_LESS "3.27")
        target_include_directories(${OCEAN_TARGET_NAME}
            PUBLIC
                "${OCEAN_IMPL_DIR}"
                ${wxWidgets_INCLUDE_DIRS}
        )
    else()
        target_include_directories(${OCEAN_TARGET_NAME}
            PUBLIC
                "${OCEAN_IMPL_DIR}"
        )
    endif()

    target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${OCEAN_TARGET_NAME} PRIVATE -DUSE_OCEAN_PLATFORM_WXWIDGETS_EXPORT)
    endif()

    target_compile_options(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_COMPILER_FLAGS})

    if (NOT WIN32)
        target_compile_options(${OCEAN_TARGET_NAME}
            PRIVATE
                "-Wno-unused-variable"
                "-Wno-shadow"
                "-Wno-global-constructors"
        )
    endif()

    # Dependencies
    target_link_libraries(${OCEAN_TARGET_NAME}
        PUBLIC
            ocean_base
            ocean_cv
            ocean_media
            ocean_platform
    )

    if(CMAKE_VERSION VERSION_LESS "3.27")
        target_link_libraries(${OCEAN_TARGET_NAME}
            PRIVATE
            ${wxWidgets_LIBRARIES}
        )
    else()
        target_link_libraries(${OCEAN_TARGET_NAME}
            PUBLIC
            wxWidgets::wxWidgets
        )
    endif()

    # Installation
    install(TARGETS ${OCEAN_TARGET_NAME}
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT lib
    )

    install(FILES ${OCEAN_TARGET_HEADER_FILES}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/platform/wxwidgets
            COMPONENT include
    )

endif()
