#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if [ -z "${ANDROID_NDK}" ]; then
  echo "ERROR: Set ANDROID_NDK to the location of your Android NDK installation."
  exit 1
fi

if [ -z "${JAVA_HOME}" ]; then
  echo "ERROR: Set JAVA_HOME to the location of your Java installation."
  exit 1
fi

OCEAN_SOURCE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )
OCEAN_BUILD_ROOT_DIRECTORY="/tmp"
OCEAN_INSTALL_ROOT_DIRECTORY="/tmp"

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build Ocean:"
    echo ""
    echo "  $(basename "$0") [-h|--help] [THIRD_PARTY_ROOT_DIRECTORY]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  THIRD_PARTY_ROOT_DIRECTORY : The optional location where the third-party libraries of Ocean are"
    echo "                               installed, if they were built manually. Otherwise standard CMake locations"
    echo "                               will be searched for compatible third-party libraries."
    echo ""
    echo "  -h | --help                : This summary"
    echo ""
}

THIRD_PARTY_ROOT_DIRECTORY=""

if [[ $# -gt 0 ]]; then
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
        ;;
        *)
        THIRD_PARTY_ROOT_DIRECTORY="${key}"
        ;;
    esac
fi

if [ -z "${THIRD_PARTY_ROOT_DIRECTORY}" ]; then
    echo "WARNING: No location for the Ocean third-party libraries has been specified; please ensure that all dependencies are satisfied."
else
    if [ ! -d "${THIRD_PARTY_ROOT_DIRECTORY}" ]; then
        echo "ERROR: The following directory for the third-party libraries cannot be found: ${THIRD_PARTY_ROOT_DIRECTORY} - did you run the script to build the third-party libraries?"
        exit 1
    fi
fi

# Builds Ocean for Android with a specific build config
#
# ANDROID_ABI: The identifier of the Android ABI for which the build will be configured, cf. https://developer.android.com/ndk/guides/abis
# ANDROID_SDK_VERSION: The version of the Android SDK as a string, format: android-X, cf. https://developer.android.com/tools/releases/platforms#12
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
function run_build_for_android {
    ANDROID_ABI=$1
    ANDROID_SDK_VERSION=$2

    BUILD_TYPE=$3
    if [[ ${BUILD_TYPE} != "Debug" ]] && [[ ${BUILD_TYPE} != "Release" ]]; then
        echo "ERROR: Invalid value: BUILD_TYPE=${BUILD_TYPE}"
        exit 1
    fi

    LIBRARY_TYPE=$4
    if [[ ${LIBRARY_TYPE} == "static" ]]; then
        ENABLE_BUILD_SHARED_LIBS="OFF"
    elif [[ ${LIBRARY_TYPE} == "shared" ]]; then
        ENABLE_BUILD_SHARED_LIBS="ON"
    else
        echo "ERROR: Invalid value: LIBRARY_TYPE=${LIBRARY_TYPE}"
        exit 1
    fi

    OCEAN_BUILD_DIRECTORY="${OCEAN_BUILD_ROOT_DIRECTORY}/ocean/build/android/${ANDROID_ABI}_${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_INSTALL_DIRECTORY="${OCEAN_INSTALL_ROOT_DIRECTORY}/ocean/install/android/${ANDROID_ABI}_${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo " "
    echo "ANDROID_ABI: ${ANDROID_ABI}"
    echo "ANDROID_SDK_VERSION: ${ANDROID_SDK_VERSION}"
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_BUILD_DIRECTORY: ${OCEAN_BUILD_DIRECTORY}"
    echo "OCEAN_INSTALL_DIRECTORY: ${OCEAN_INSTALL_DIRECTORY}"
    echo " "

    CMAKE_CONFIGURE_COMMAND="cmake \\
    -S\"${OCEAN_SOURCE_DIRECTORY}\" \\
    -B\"${OCEAN_BUILD_DIRECTORY}\" \\
    -DCMAKE_BUILD_TYPE=\"${BUILD_TYPE}\" \\
    -DANDROID_ABI=\"${ANDROID_ABI}\" \\
    -DANDROID_PLATFORM=\"${ANDROID_SDK_VERSION}\" \\
    -DCMAKE_ANDROID_STL=c++_static \\
    -DCMAKE_ANDROID_NDK=\"${ANDROID_NDK}\" \\
    -DCMAKE_TOOLCHAIN_FILE=\"${ANDROID_NDK}/build/cmake/android.toolchain.cmake\" \\
    -DCMAKE_INSTALL_PREFIX=\"${OCEAN_INSTALL_DIRECTORY}\" \\
    -DBUILD_SHARED_LIBS=\"${ENABLE_BUILD_SHARED_LIBS}\""

    if [ -n "${THIRD_PARTY_ROOT_DIRECTORY}" ]; then
        OCEAN_THIRD_PARTY_DIRECTORY="${THIRD_PARTY_ROOT_DIRECTORY}/${ANDROID_ABI}_${LIBRARY_TYPE}_${BUILD_TYPE}"

        echo "OCEAN_THIRD_PARTY_DIRECTORY: ${OCEAN_THIRD_PARTY_DIRECTORY}"
        echo " "

        # The Android NDK makes it so that CMake ignore CMAKE_PREFIX_PATH; because of that CMAKE_FIND_ROOT_PATH needs to be set. Details:
        # https://discourse.cmake.org/t/cmake-does-not-find-libraries-when-compiling-for-android/5098/5
        CMAKE_CONFIGURE_COMMAND+="  \\
    -DCMAKE_PREFIX_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\" \\
    -DCMAKE_MODULE_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\" \\
    -DCMAKE_FIND_ROOT_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\""
    fi

    echo "CMAKE_CONFIGURE_COMMAND = ${CMAKE_CONFIGURE_COMMAND}"
    eval "${CMAKE_CONFIGURE_COMMAND}"

    cmake --build "${OCEAN_BUILD_DIRECTORY}" --target install -- -j16

    echo " "
    echo " "
    echo " "
}

run_build_for_android armeabi-v7a android-32 Debug static
run_build_for_android arm64-v8a   android-32 Debug static
run_build_for_android x86         android-32 Debug static
run_build_for_android x86_64      android-32 Debug static

run_build_for_android armeabi-v7a android-32 Debug shared
run_build_for_android arm64-v8a   android-32 Debug shared
run_build_for_android x86         android-32 Debug shared
run_build_for_android x86_64      android-32 Debug shared

run_build_for_android armeabi-v7a android-32 Release static
run_build_for_android arm64-v8a   android-32 Release static
run_build_for_android x86         android-32 Release static
run_build_for_android x86_64      android-32 Release static

run_build_for_android armeabi-v7a android-32 Release shared
run_build_for_android arm64-v8a   android-32 Release shared
run_build_for_android x86         android-32 Release shared
run_build_for_android x86_64      android-32 Release shared
