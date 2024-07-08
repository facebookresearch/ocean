#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if [[ "$(uname)" != "Darwin" ]]; then
    echo "ERROR: This script can only be used on macOS."
    exit 1
fi

OCEAN_PLATFORM="ios"

# The flag indicating the platform that will be built for, cf. ios.toolchain.cmake for details
# OS64 - build for iOS (arm64 only)
IOS_CMAKE_TOOLCHAIN_PLATFORM="OS64"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# OTP = OCEAN_THIRD_PARTY
OTP_SOURCE_DIR=$( cd "${SCRIPT_DIR}/third-party" && pwd )

OTP_BUILD_DIR="${PWD}/ocean_build_thirdparty"
OTP_INSTALL_DIR="${PWD}/ocean_install_thirdparty"

OTP_VALID_BUILD_CONFIGS="debug,release"
OTP_BUILD_CONFIG="release"

OTP_VALID_LINKING_TYPES="static,shared"
OTP_LINKING_TYPES="static"

IOS_CMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/ios-cmake/ios.toolchain.cmake"

if ! [ -f "${IOS_CMAKE_TOOLCHAIN_FILE}" ]; then
  echo "ERROR: Cannot find the toolchain file that's required for iOS builds."
  exit 1
fi

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build the third-party libraries required by Ocean (${OCEAN_PLATFORM}):"
    echo ""
    echo "  $(basename "$0") [-h|--help] [-i|--install INSTALL_DIR] [-b|--build BUILD_DIR] [-c|--config BUILD_CONFIG]"
    echo "                   [-l|--link LINKING_TYPE] [-a | --archive ARCHIVE]"
    echo ""
    echo "Arguments:"
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
    echo "                specified: \"${OTP_BUILD_CONFIG}\""
    echo ""
    echo "  -l | --link LINKING_TYPE : The optional linking type for which will be built; valid values are:"
    for type in $(echo "${OTP_VALID_LINKING_TYPES}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. Default value if nothing is"
    echo "                specified: \"${OTP_LINKING_TYPES}\""
    echo ""
    echo "  -h | --help : This summary"
    echo ""
}

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
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
        OTP_BUILD_CONFIG="$2"
        shift # past argument
        shift # past value
        ;;
        -l|--link)
        OTP_LINKING_TYPES="$2"
        shift # past argument
        shift # past value
        ;;
        *)
        echo "ERROR: Unknown value \"$1\"." >&2
        exit 1
        ;;
    esac
done

# Builds the third-party libraries for Ocean (iOS)
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

    # Specific build and installation directory for the current build config
    BUILD_DIR="${OTP_BUILD_DIR}/${OCEAN_PLATFORM}_${IOS_CMAKE_TOOLCHAIN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"
    INSTALL_DIR="${OTP_INSTALL_DIR}/${OCEAN_PLATFORM}_${IOS_CMAKE_TOOLCHAIN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"

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

    BUILD_COMMAND="\"${OTP_SOURCE_DIR}/build_deps.sh\" ios \"${OTP_SOURCE_DIR}\" \"${BUILD_DIR}\" \"-parallelizeTargets -jobs $(sysctl -n hw.ncpu) CODE_SIGNING_ALLOWED=NO\" \
        \"-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}\" \
        -GXcode \
        \"-DCMAKE_TOOLCHAIN_FILE=${IOS_CMAKE_TOOLCHAIN_FILE}\" \
        \"-DPLATFORM=${IOS_CMAKE_TOOLCHAIN_PLATFORM}\" \
        \"-DDEPLOYMENT_TARGET=15\" \
        \"-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}\" \
        \"-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}\" \
        \"-DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER:STRING=org.ocean.thirdparty\""

    eval "${BUILD_COMMAND}"
    if [ "$?" != 0 ]; then
        OTP_FAILED_BUILDS+=("${LINKING_TYPE} + ${BUILD_CONFIG}")
    fi

    echo ""
    echo ""
    echo ""
}

echo "Building the third-party libraries required for Ocean (${OCEAN_PLATFORM}) ...:"
echo ""

if [ "${OTP_BUILD_CONFIG}" == "" ]; then
    echo "ERROR: At least one build type has to be specified." >&2
    exit 1
fi

# Remove duplicate values
OTP_BUILD_CONFIG=$(echo "$OTP_BUILD_CONFIG" | tr ',' '\n' | sort -u)

# Only allow valid values
for type in ${OTP_BUILD_CONFIG}; do
    if ! echo "${OTP_VALID_BUILD_CONFIGS}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

if [ "${OTP_LINKING_TYPES}" == "" ]; then
    echo "ERROR: At least one linking type has to be specified." >&2
    exit 1
fi

# Remove duplicate values
OTP_LINKING_TYPES=$(echo "$OTP_LINKING_TYPES" | tr ',' '\n' | sort -u)

# Only allow valid values
for type in ${OTP_LINKING_TYPES}; do
    if ! echo "${OTP_VALID_LINKING_TYPES}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

echo "The third-party libraries will be build for the following combinations:"
for build_config in ${OTP_BUILD_CONFIG}; do
    for link_type in ${OTP_LINKING_TYPES}; do
        echo " * ${build_config} + ${link_type}"
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
for build_config in ${OTP_BUILD_CONFIG}; do
    for link_type in ${OTP_LINKING_TYPES}; do
        run_build "${build_config}" "${link_type}"
    done
done

# Determine if all of the above builds were successful.
OTP_BUILD_SUCCESSFUL=0

if [ "${#OTP_FAILED_BUILDS[@]}" -eq 0 ]; then
    OTP_BUILD_SUCCESSFUL=1
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
