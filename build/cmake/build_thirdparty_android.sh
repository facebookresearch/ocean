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

# OTP = OCEAN_THIRD_PARTY
OTP_SOURCE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd third-party && pwd )

OTP_BUILD_DIR="${PWD}/ocean_build_thirdparty"
OTP_INSTALL_DIR="${PWD}/ocean_install_thirdparty"

OTP_VALID_ANDROID_ABIS="arm64-v8a,armeabi-v7a,x86_64,x86"
OTP_ANDROID_ABIS="arm64-v8a"

OTP_VALID_BUILD_CONFIGS="debug,release"
OTP_BUILD_CONFIGS="debug,release"

OTP_VALID_LINKING_TYPES="static,shared"
OTP_LINKING_TYPES="static"

OTP_ANDROID_SDK="android-32"

OTP_ARCHIVE=""

# Collection of builds that have errors that will be listed at the end of the script
OTP_FAILED_BUILDS=()

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build the third-party libraries required by Ocean (${OCEAN_PLATFORM}):"
    echo ""
    echo "  $(basename "$0") [-h|--help] [-i|--install INSTALL_DIR] [-b|--build BUILD_DIR] [-c|--config BUILD_CONFIG]"
    echo "                   [-l|--link LINKING_TYPE] [-a | --archive ARCHIVE] [-p|--abi ABI_LIST]"
    echo "                   [-s|--sdk ANDROID_SDK]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -p | --abi ABI_LIST : A list of Android ABIs as build target platforms; valid values are:"
    for abi in $(echo "${OTP_VALID_ANDROID_ABIS}" | tr ',' '\n'); do
        echo "                  ${abi}"
    done
    echo "                The default is: ${OTP_ANDROID_ABIS}"
    echo ""
    echo "  -s | --sdk ANDROID_SDK : name of Android SDK version for builds. Default: ${OTP_ANDROID_SDK}"
    echo ""
    echo "  -i | --install INSTALL_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be installed. Default installation directory:"
    echo "                ${OTP_INSTALL_DIR}"
    echo ""
    echo "  -b | --build BUILD_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be built. Default build directory:"
    echo "                ${OTP_BUILD_DIR}"
    echo ""
    echo "  -c | --config BUILD_CONFIG : The optional build configs(s) to be built; valid values are:"
    for type in $(echo "${OTP_VALID_BUILD_CONFIGS}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. Default value if nothing is"
    echo "                specified: \"${OTP_BUILD_CONFIGS}\""
    echo ""
    echo "  -l | --link LINKING_TYPE : The optional linking type for which will be built; valid values are:"
    for type in $(echo "${OTP_VALID_LINKING_TYPES}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. Default value if nothing is"
    echo "                specified: \"${OTP_LINKING_TYPES}\""
    echo ""
    echo " -a | --archive ARCHIVE: If specified, this will copy the contents of INSTALL_DIR after the build"
    echo "                into a ZIP archive; the path to this archive must exist."
    echo ""
    echo "  -h | --help : This summary"
    echo ""
}

# Builds the third-party libraries for Ocean
#
# BUILD_CONFIG: The build type to be used, valid values: Debug, Release
# LINKING_TYPE: The type of libraries to be built, valid values: static, shared
function run_build {
    BUILD_CONFIG=$1

    # Convert the name of the build mode to the CMake notation.
    if [[ ${BUILD_CONFIG} == "debug" ]]; then
        BUILD_CONFIG="Debug"
    elif [[ ${BUILD_CONFIG} == "release" ]]; then
        BUILD_CONFIG="Release"
    else
        echo "ERROR: Invalid value: BUILD_CONFIG=${BUILD_CONFIG}" >&2
        exit 1
    fi

    LINKING_TYPE=$2
    if [[ ${LINKING_TYPE} == "static" ]]; then
        ENABLE_BUILD_SHARED_LIBS="OFF"
    elif [[ ${LINKING_TYPE} == "shared" ]]; then
        ENABLE_BUILD_SHARED_LIBS="ON"
    else
        echo "ERROR: Invalid value: LINKING_TYPE=${LINKING_TYPE}" >&2
        exit 1
    fi

    ANDROID_ABI=$3
    if ! grep -q "${ANDROID_ABI}" <<< "${OTP_VALID_ANDROID_ABIS}" ; then
        echo "ERROR: Invalid Android ABI ${ANDROID_ABI}" >&2
        exit 1
    fi

    ANDROID_SDK_VERSION=$4

    BUILD_DIR="${OTP_BUILD_DIR}/${OCEAN_PLATFORM}_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_CONFIG}"
    INSTALL_DIR="${OTP_INSTALL_DIR}/${OCEAN_PLATFORM}_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_CONFIG}"

    echo ""
    echo ""
    echo ""
    echo "Build type: ${BUILD_CONFIG}"
    echo "Linking type: ${LINKING_TYPE}"
    echo ""
    echo "Build directory: ${BUILD_DIR}"
    echo "Install directory: ${INSTALL_DIR}"
    echo ""
    echo ""
    echo ""

    eval "${OTP_SOURCE_DIR}/build_deps.sh" ${OCEAN_PLATFORM} "${OTP_SOURCE_DIR}" "${BUILD_DIR}" -j16 \
        "-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}" \
        "-DANDROID_ABI=${ANDROID_ABI}" \
        "-DANDROID_PLATFORM=${ANDROID_SDK_VERSION}" \
        "-DCMAKE_ANDROID_STL_TYPE=c++_static" \
        "-DCMAKE_ANDROID_NDK=${ANDROID_NDK}" \
        "-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake" \
        "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}" \
        "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}"
    if [ "$?" != 0 ]; then
        OTP_FAILED_BUILDS+=("${ANDROID_ABI} + ${LINKING_TYPE} + ${BUILD_CONFIG}")
    fi

    echo ""
    echo ""
    echo ""
}

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
        ;;
        -p|--abi)
        OTP_ANDROID_ABIS="$2"
        shift
        shift
        ;;
        -s|--sdk)
        OTP_ANDROID_SDK="$2"
        shift
        shift
        ;;
        -i|--install)
        OTP_INSTALL_DIR="$2"
        shift # past argument
        shift # past value
        ;;
        -b|--build)
        OTP_BUILD_DIR="$2"
        shift # past argument
        shift # past value
        ;;
        -c|--config)
        OTP_BUILD_CONFIGS="$2"
        shift # past argument
        shift # past value
        ;;
        -l|--link)
        OTP_LINKING_TYPES="$2"
        shift # past argument
        shift # past value
        ;;
        -a|--archive)
        OTP_ARCHIVE="$2"
        shift # past argument
        shift # past value
        ;;
        *)
        echo "ERROR: Unknown value \"$1\"." >&2
        exit 1
        ;;
    esac
done

echo "Building the third-party libraries required for Ocean (${OCEAN_PLATFORM}) ...:"
echo ""

if [ "${OTP_BUILD_CONFIGS}" == "" ]; then
    echo "ERROR: At least one build type has to be specified." >&2
    exit 1
fi

# Build configs: remove duplicate values and only allow valid values
OTP_BUILD_CONFIGS=$(echo "${OTP_BUILD_CONFIGS}" | tr ',' '\n' | sort -u)

for type in ${OTP_BUILD_CONFIGS}; do
    if ! echo "${OTP_VALID_BUILD_CONFIGS}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

if [ "${OTP_ANDROID_ABIS}" == "" ]; then
    echo "ERROR: At least one Android ABI has to be specified." >&2
    exit 1
fi

# Android ABIs: remove duplicate values and only allow valid values
OTP_ANDROID_ABIS=$(echo "${OTP_ANDROID_ABIS}" | tr ',' '\n' | sort -u)

for type in ${OTP_ANDROID_ABIS}; do
    if ! echo "${OTP_VALID_ANDROID_ABIS}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown Android ABI \"${type}\"" >&2
        exit 1
    fi
done

if [ "${OTP_LINKING_TYPES}" == "" ]; then
    echo "ERROR: At least one linking type has to be specified." >&2
    exit 1
fi

# Linking types: remove duplicate values and only allow valid values
OTP_LINKING_TYPES=$(echo "$OTP_LINKING_TYPES" | tr ',' '\n' | sort -u)

for type in ${OTP_LINKING_TYPES}; do
    if ! echo "${OTP_VALID_LINKING_TYPES}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

echo "The third-party libraries will be built for the following combinations:"
for abi in ${OTP_ANDROID_ABIS}; do
    for build_config in ${OTP_BUILD_CONFIGS}; do
        for link_type in ${OTP_LINKING_TYPES}; do
            echo " * ${abi} + ${build_config} + ${link_type}"
        done
    done
done

echo ""
echo ""
echo ""
echo "Install root directory for all builds: ${OTP_INSTALL_DIR}"
echo ""
echo ""
echo ""

# Build
for abi in ${OTP_ANDROID_ABIS}; do
    for build_config in ${OTP_BUILD_CONFIGS}; do
        for link_type in ${OTP_LINKING_TYPES}; do
            run_build "${build_config}" "${link_type}" "${abi}" "${OTP_ANDROID_SDK}"
        done
    done
done

# Determine if all of the above builds were successful.
OTP_BUILD_SUCCESSFUL=0

if [ "${#OTP_FAILED_BUILDS[@]}" -eq 0 ]; then
    OTP_BUILD_SUCCESSFUL=1
fi

# Copy the build artifacts into the specified archive, if applicable
if [ "${OTP_ARCHIVE}" != "" ]; then
    if [[ ${OTP_BUILD_SUCCESSFUL} == 1 && -d ${OTP_INSTALL_DIR} ]]; then
        echo "Creating \"${OTP_ARCHIVE}\" ..."
        OTP_CURRENT_DIRECTORY="${PWD}"
        cd  "${OTP_INSTALL_DIR}" && zip -rv "${OTP_ARCHIVE}" .
        cd "${OTP_CURRENT_DIRECTORY}"
        echo "done."
        echo ""
    else
        echo "WARNING: Failed to create \"${OTP_ARCHIVE}\"."
    fi
fi

if [ ${OTP_BUILD_SUCCESSFUL} == 1 ]; then
    echo "All builds were successful."
else
    echo "Some builds have failed." >&2
    for config in "${OTP_FAILED_BUILDS[@]}"; do
        echo "- $config" >&2
    done

    exit 1
fi
