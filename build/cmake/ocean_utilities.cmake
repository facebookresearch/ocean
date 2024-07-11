# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# The builtin variable APPLE is set to true for both, macOS and iOS. While there is a dedicated
# builtin variable for iOS (IOS), there is no such variable for only macOS. So, it's added here.
if (APPLE AND (${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
    set(MACOS TRUE)
endif()

if (IOS OR MACOS)
    # Enable CMake language support Objective-C/C++ for IOS and MACOS.
    # Some third-party builds (e.g. OpenCV) need this enabled to build successfully.
    # Some Ocean build targets need this enabled in order to apply compiler options intended for Objective-C/C++ files.
    enable_language(OBJC OBJCXX)
endif()

# In order to be able to differentiate whether this build is intended for standard Android or for Android for Quest,
# the user can specify the parameter OCEAN_ENABLE_QUEST to CMake at configure time. This is mostly relevant for
# the native libraries of Android apps and some other platform-specific libraries.
if (ANDROID)
    if (OCEAN_ENABLE_QUEST)
        set(QUEST TRUE)
    endif()
else()
    if (OCEAN_ENABLE_QUEST)
        message(WARNING "Specifying -DOCEAN_ENABLE_QUEST has no effect and will be ignored; it is only meant for Android builds.")
    endif()
endif()

# Translates the system names CMake uses to system names Ocean prefers
#
# The mapping is defined as follows (CMake -> Ocean):
#   Android -> android
#   Darwin -> macos
#   Linux -> linux
#   Windows -> win
#
# Parameters:
#   system_name_string: The name of the system that will be translated as a string, must be defined
#
# Output:
#   translated_system_name_variable: The name of variable where the result should be stored
#
# The function will fail is the CMake system name is undefined.
#
# Example: ocean_translate_system_name("Darwin" output_variable)
function(ocean_translate_system_name system_name_string translated_system_name_variable)
    if (${system_name_string} STREQUAL "Darwin")
        set(${translated_system_name_variable} "macos" PARENT_SCOPE)
    elseif (${system_name_string} STREQUAL "iOS")
        set(${translated_system_name_variable} "ios" PARENT_SCOPE)
    elseif (${system_name_string} STREQUAL "Windows")
        set(${translated_system_name_variable} "win" PARENT_SCOPE)
    elseif (${system_name_string} STREQUAL "Linux")
        set(${translated_system_name_variable} "linux" PARENT_SCOPE)
    elseif (${system_name_string} STREQUAL "Android")
        set(${translated_system_name_variable} "android" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown/unsupported system name: ${system_name_string}")
    endif()
endfunction()

# Creates the default preprocessor flags for Ocean
#
# Output:
#   ocean_preprocessor_flags: The default preprocessor flags for Ocean
function(get_ocean_preprocessor_flags ocean_preprocessor_flags)
    set(local_preprocessor_flags "")

    if (OCEAN_ENABLE_MESSENGER)
        list(APPEND local_preprocessor_flags "-DOCEAN_ACTIVATE_MESSENGER=1")
    endif()

    if (BUILD_SHARED_LIBS)
        list(APPEND local_preprocessor_flags "-DOCEAN_RUNTIME_SHARED")
    else()
        list(APPEND local_preprocessor_flags "-DOCEAN_RUNTIME_STATIC")
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND local_preprocessor_flags "-DDEBUG")
        list(APPEND local_preprocessor_flags "-DENABLE_LOGGING=1")

        if (WIN32)
            list(APPEND local_preprocessor_flags "-D_DEBUG")
            list(APPEND local_preprocessor_flags "-DEXTENDED_DEBUG")
        endif()

    endif()

    if (WIN32)
        list(APPEND local_preprocessor_flags "-D_WINDOWS")
        list(APPEND local_preprocessor_flags "-DWIN32")
        list(APPEND local_preprocessor_flags "-D_CRT_SECURE_NO_DEPRECATE")
    endif()

    set(${ocean_preprocessor_flags} ${local_preprocessor_flags} PARENT_SCOPE)
endfunction()

# Creates the default compiler flags for Ocean
#
# Output:
#   ocean_compiler_flags: The default compilers flags for Ocean
function(get_ocean_compiler_flags ocean_compiler_flags)
    set(local_compiler_flags "")

    if (MACOS)
        if (OCEAN_TARGET_PROCESSOR_TYPE STREQUAL "x86_64")
            list(APPEND local_compiler_flags "-msse4.1")
            list(APPEND local_compiler_flags "-mavx2")
        elseif(OCEAN_TARGET_PROCESSOR_TYPE STREQUAL "arm64")
            # Nothing else to add at this time.
        else()
            message(FATAL_ERROR "Unsupported Apple CPU architecture.")
        endif()
    elseif (IOS)
        # Nothing else to add at this time.
    elseif (ANDROID)
        list(APPEND local_compiler_flags "-D_ANDROID")
        list(APPEND local_compiler_flags "-frtti")
        list(APPEND local_compiler_flags "-fsigned-char")
    elseif (WIN32)
        list(APPEND local_compiler_flags "/MP")
        list(APPEND local_compiler_flags "/W4")
        list(APPEND local_compiler_flags "/WX-")
        list(APPEND local_compiler_flags "/permissive-")
        list(APPEND local_compiler_flags "/w15038") # C5038: data member 'member1' will be initialized after data member 'member2'
        list(APPEND local_compiler_flags "/w34265") # C4265: 'class': class has virtual functions, but destructor is not virtual
        list(APPEND local_compiler_flags "/w44062") # C4062: enumerator 'identifier' in switch of enum 'enumeration' is not handled
        list(APPEND local_compiler_flags "/w45266") # C5266: 'const' qualifier on return type has no effect
        list(APPEND local_compiler_flags "/Zo")
        # Disabled warnings
        list(APPEND local_compiler_flags "/wd4121") # C4121: alignment of a member was sensitive to packing
        list(APPEND local_compiler_flags "/wd4127") # C4127: conditional expression is constant (also warns when constexpr and not-constexpr in the same condition)
        list(APPEND local_compiler_flags "/wd4250") # C4250: inherits class member via dominance
        list(APPEND local_compiler_flags "/wd4251") # C4251: class needs to have dll-interface to be used by clients of class
        list(APPEND local_compiler_flags "/wd4275") # C4275: non dll-interface class used as base for dll-interface class
        list(APPEND local_compiler_flags "/wd4355") # C4355: 'this' used in base member initializer list
        list(APPEND local_compiler_flags "/wd4589") # C4589: Constructor of abstract class 'type' ignores initializer for virtual base class 'type' (unclear how to prevent the warning)
        list(APPEND local_compiler_flags "/wd4702") # C4702: unreachable code (sometimes intended for readability e.g., due to platform independent code)
        # Unicode
        list(APPEND local_compiler_flags "/UMBCS")
        list(APPEND local_compiler_flags "/D_UNICODE")
        list(APPEND local_compiler_flags "/DUNICODE")
    elseif (LINUX)
        if (OCEAN_TARGET_PROCESSOR_TYPE STREQUAL "x86_64")
            list(APPEND local_compiler_flags "-msse4.1")
            list(APPEND local_compiler_flags "-mavx2")
        endif()
    endif()

    set(${ocean_compiler_flags} ${local_compiler_flags} PARENT_SCOPE)
endfunction()
