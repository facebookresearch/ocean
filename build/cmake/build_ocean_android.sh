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

OCEAN_PLATFORM="android"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OCEAN_SOURCE_DIR=$( cd "${SCRIPT_DIR}" && cd ../.. && pwd )

OCEAN_BUILD_DIR="${PWD}/ocean_build"
OCEAN_INSTALL_DIR="${PWD}/ocean_install"

OCEAN_VALID_BUILD_CONFIGS="debug,release"
OCEAN_BUILD_CONFIGS="release"

OCEAN_VALID_LINKING_TYPES="static,shared"
OCEAN_LINKING_TYPES="static"

OCEAN_VALID_ANDROID_ABIS="arm64-v8a,armeabi-v7a,x86_64,x86"
OCEAN_ANDROID_ABIS="arm64-v8a"

OCEAN_ANDROID_SDK="android-32"

OCEAN_ENABLE_QUEST=""

OCEAN_THIRD_PARTY_DIR=""

# Collection of builds that have errors that will be listed at the end of the script
OCEAN_FAILED_BUILDS=()

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build Ocean (${OCEAN_PLATFORM}):"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  $(basename "$0") [-h|--help] [-i|--install INSTALL_DIR] [-b|--build BUILD_DIR] [-c|--config BUILD_CONFIG]"
    echo "                   [-l|--link LINKING_TYPE] [-p|--abi ABI_LIST] [-s|--sdk ANDROID_SDK] [-q | --quest]"
    echo "                   [-t|--third-party OCEAN_THIRD_PARTY_DIR]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -i | -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be installed. Default installation directory: ${OCEAN_INSTALL_DIR}"
    echo ""
    echo "  -b | -build BUILD_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be built. Default build directory: ${OCEAN_BUILD_DIR}"
    echo ""
    echo "  -c | --config BUILD_CONFIG : The optional build configs(s) to be built; valid values are:"
    for type in $(echo "${OCEAN_VALID_BUILD_CONFIGS}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. Default value if nothing is"
    echo "                specified: \"${OCEAN_BUILD_CONFIGS}\""
    echo ""
    echo "  -l | -link LINKING_TYPE : The optional linking type for which will be built; valid values are:"
    for type in $(echo "${OCEAN_VALID_LINKING_TYPES}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. Default value if nothing is"
    echo "                specified: \"${OCEAN_LINKING_TYPES}\""
    echo ""
    echo "  -p | --abi ABI_LIST : A list of Android ABIs as build target platforms; valid values are:"
    for abi in $(echo "${OCEAN_VALID_ANDROID_ABIS}" | tr ',' '\n'); do
        echo "                  ${abi}"
    done
    echo "                The default is: ${OCEAN_ANDROID_ABIS}"
    echo ""
    echo "  -s | --sdk ANDROID_SDK : name of Android SDK version for builds. Default: ${OCEAN_ANDROID_SDK}"
    echo ""
    echo "  -q | --quest : If specified, builds will be specialized for Quest apps. Otherwise standard Android is assumed."
    echo ""
    echo "  -t | --third-party : The location where the third-party libraries of Ocean are located, if they"
    echo "                were built manually. Otherwise standard CMake locations will be search for"
    echo "                compatible third-party libraries."
    echo ""
    echo "  -h | --help : This summary"
    echo ""
    echo "  -h | --help                : This summary"
    echo ""
}

# Builds Ocean for Android with a specific build config
#
# ANDROID_ABI: The identifier of the Android ABI for which the build will be configured, cf. https://developer.android.com/ndk/guides/abis
# ANDROID_SDK_VERSION: The version of the Android SDK as a string, format: android-X, cf. https://developer.android.com/tools/releases/platforms#12
# BUILD_CONFIG: The build type to be used, valid values: Debug, Release
# LINKING_TYPE: The type of libraries to be built, valid values: static, shared
function run_build {
    ANDROID_ABI=$1
    ANDROID_SDK_VERSION=$2

    BUILD_CONFIG=$3

    # Convert the name of the build mode to the CMake notation.
    if [[ ${BUILD_CONFIG} == "debug" ]]; then
        BUILD_CONFIG="Debug"
    elif [[ ${BUILD_CONFIG} == "release" ]]; then
        BUILD_CONFIG="Release"
    else
        echo "ERROR: Invalid value: BUILD_CONFIG=${BUILD_CONFIG}" >&2
        exit 1
    fi

    LINKING_TYPE=$4
    if [[ ${LINKING_TYPE} == "static" ]]; then
        ENABLE_BUILD_SHARED_LIBS="OFF"
    elif [[ ${LINKING_TYPE} == "shared" ]]; then
        ENABLE_BUILD_SHARED_LIBS="ON"
    else
        echo "ERROR: Invalid value: LINKING_TYPE=${LINKING_TYPE}" >&2
        exit 1
    fi

    BUILD_DIR="${OCEAN_BUILD_DIR}/${OCEAN_PLATFORM}_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_CONFIG}"
    INSTALL_DIR="${OCEAN_INSTALL_DIR}/${OCEAN_PLATFORM}_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_CONFIG}"

    echo " "
    echo "ANDROID_ABI: ${ANDROID_ABI}"
    echo "ANDROID_SDK_VERSION: ${ANDROID_SDK_VERSION}"
    echo "BUILD_CONFIG: ${BUILD_CONFIG}"
    echo "LINKING_TYPE: ${LINKING_TYPE}"
    echo " "
    echo "BUILD_DIR: ${BUILD_DIR}"
    echo "INSTALL_DIR: ${INSTALL_DIR}"
    echo " "

    CMAKE_CONFIGURE_COMMAND="cmake \\
    -S\"${OCEAN_SOURCE_DIR}\" \\
    -B\"${BUILD_DIR}\" \\
    -DCMAKE_BUILD_TYPE=\"${BUILD_CONFIG}\" \\
    -DANDROID_ABI=\"${ANDROID_ABI}\" \\
    -DANDROID_PLATFORM=\"${ANDROID_SDK_VERSION}\" \\
    -DCMAKE_ANDROID_STL_TYPE=c++_static \\
    -DCMAKE_ANDROID_NDK=\"${ANDROID_NDK}\" \\
    -DCMAKE_TOOLCHAIN_FILE=\"${ANDROID_NDK}/build/cmake/android.toolchain.cmake\" \\
    -DCMAKE_INSTALL_PREFIX=\"${INSTALL_DIR}\" \\
    -DBUILD_SHARED_LIBS=\"${ENABLE_BUILD_SHARED_LIBS}\""

    if [ -n "${OCEAN_THIRD_PARTY_DIR}" ]; then
        # This must match the INSTALL_DIR from ./build_thirdparty_android.sh
        THIRD_PARTY_DIR="${OCEAN_THIRD_PARTY_DIR}/${OCEAN_PLATFORM}_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_CONFIG}"

        echo "THIRD_PARTY_DIR: ${THIRD_PARTY_DIR}"
        echo " "

        # The Android NDK makes it so that CMake ignore CMAKE_PREFIX_PATH; because of that CMAKE_FIND_ROOT_PATH needs to be set. Details:
        # https://discourse.cmake.org/t/cmake-does-not-find-libraries-when-compiling-for-android/5098/5
        CMAKE_CONFIGURE_COMMAND+="  \\
    -DCMAKE_PREFIX_PATH=\"${THIRD_PARTY_DIR}\" \\
    -DCMAKE_MODULE_PATH=\"${THIRD_PARTY_DIR}\" \\
    -DCMAKE_FIND_ROOT_PATH=\"${THIRD_PARTY_DIR}\""
    fi

    if [ -n "${OCEAN_ENABLE_QUEST}" ]; then
        # Signal to CMake that this build is intended for Android for Quest, not standard Android.
        CMAKE_CONFIGURE_COMMAND+="  \\
    -DOCEAN_ENABLE_QUEST=TRUE\""
    fi

    echo "CMAKE_CONFIGURE_COMMAND = ${CMAKE_CONFIGURE_COMMAND}"
    eval "${CMAKE_CONFIGURE_COMMAND}"

    if ! cmake --build "${BUILD_DIR}" --target install -- -j16; then
        OCEAN_FAILED_BUILDS+=("${ANDROID_ABI} + ${LINKING_TYPE} + ${BUILD_CONFIG}")
    fi

    echo " "
    echo " "
    echo " "
}

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
        ;;
        -i|--install)
        OCEAN_INSTALL_DIR="$2"
        shift # past argument
        shift # past value
        ;;
        -b|--build)
        OCEAN_BUILD_DIR="$2"
        shift # past argument
        shift # past value
        ;;
        -c|--config)
        OCEAN_BUILD_CONFIGS="$2"
        shift # past argument
        shift # past value
        ;;
        -l|--link)
        OCEAN_LINKING_TYPES="$2"
        shift # past argument
        shift # past value
        ;;
        -t|--third-party)
        OCEAN_THIRD_PARTY_DIR="$2"
        shift # past argument
        shift # past value
        ;;
        -p|--abi)
        OCEAN_ANDROID_ABIS="$2"
        shift
        shift
        ;;
        -s|--sdk)
        OCEAN_ANDROID_SDK="$2"
        shift
        shift
        ;;
        -q|--quest)
        OCEAN_ENABLE_QUEST=1
        shift
        ;;
        *)
        echo "ERROR: Unknown value \"$1\"." >&2
        exit 1
        ;;
    esac
done

if [ "${OCEAN_THIRD_PARTY_DIR}" == "" ]; then
    echo ""
    echo ""
    echo "WARNING: No location for the Ocean third-party libraries was specified. If this wasn't intentional make sure to build the third-party libraries first. Please refer to the build instructions for details." >&2
    echo ""
    echo ""
else
    if [ ! -d "${OCEAN_THIRD_PARTY_DIR}" ]; then
        echo "ERROR: The following directory for the third-party libraries cannot be found: ${OCEAN_THIRD_PARTY_DIR} - did you run the script to build the third-party libraries?" >&2
        exit 1
    fi
fi

if [ "${OCEAN_BUILD_CONFIGS}" == "" ]; then
    echo "ERROR: At least one build type has to be specified." >&2
    exit 1
fi

# Build configs: remove duplicate values and only allow valid values
OCEAN_BUILD_CONFIGS=$(echo "$OCEAN_BUILD_CONFIGS" | tr ',' '\n' | sort -u)

for type in ${OCEAN_BUILD_CONFIGS[@]}; do
    if ! echo "${OCEAN_VALID_BUILD_CONFIGS}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

if [ "${OCEAN_LINKING_TYPES}" == "" ]; then
    echo "ERROR: At least one linking type has to be specified." >&2
    exit 1
fi

# Linking types: remove duplicate values and only allow valid values
OCEAN_LINKING_TYPES=$(echo "$OCEAN_LINKING_TYPES" | tr ',' '\n' | sort -u)

for type in ${OCEAN_LINKING_TYPES[@]}; do
    if ! echo "${OCEAN_VALID_LINKING_TYPES}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

# Android ABIs: remove duplicate values and only allow valid values
OCEAN_ANDROID_ABIS=$(echo "${OCEAN_ANDROID_ABIS}" | tr ',' '\n' | sort -u)

for type in ${OCEAN_ANDROID_ABIS}; do
    if ! echo "${OCEAN_VALID_ANDROID_ABIS}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown Android ABI \"${type}\"" >&2
        exit 1
    fi
done

if [ "${OCEAN_LINKING_TYPES}" == "" ]; then
    echo "ERROR: At least one linking type has to be specified." >&2
    exit 1
fi

echo "The Ocean libraries will be built for the following combinations:"
for android_abi in ${OCEAN_ANDROID_ABIS}; do
    for build_config in ${OCEAN_BUILD_CONFIGS}; do
        for link_type in ${OCEAN_LINKING_TYPES}; do
            echo " * ${android_abi} + ${OCEAN_ANDROID_SDK} + ${build_config} + ${link_type}"
        done
    done
done

echo ""
echo ""
echo ""
echo "Build directory: ${OCEAN_INSTALL_DIR}"
echo "Installation directory: ${OCEAN_INSTALL_DIR}"
echo ""
echo ""
echo ""

# Build
for android_abi in ${OCEAN_ANDROID_ABIS}; do
    for build_config in ${OCEAN_BUILD_CONFIGS}; do
        for link_type in ${OCEAN_LINKING_TYPES}; do
            run_build "${android_abi}" "${OCEAN_ANDROID_SDK}" "${build_config}" "${link_type}"
        done
    done
done

# Determine if all of the above builds were successful.
OCEAN_BUILD_SUCCESSFUL=0

if [ "${#OCEAN_FAILED_BUILDS[@]}" -eq 0 ]; then
    OCEAN_BUILD_SUCCESSFUL=1
fi

if [ ${OCEAN_BUILD_SUCCESSFUL} -eq 1 ]; then
    echo "All builds were successful."
else
    echo "Some builds have failed." >&2
    for config in "${OCEAN_FAILED_BUILDS[@]}"; do
        echo "- $config" >&2
    done

    exit 1
fi
