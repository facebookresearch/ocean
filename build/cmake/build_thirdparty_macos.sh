#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

echo "Building the third-party libraries required for Ocean ...:"
echo ""

OCEAN_PLATFORM="macos"

# OTP = OCEAN_THIRD_PARTY
OTP_SOURCE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd third-party && pwd )

OTP_BUILD_DIR="/tmp/ocean/build/${OCEAN_PLATFORM}"
OTP_INSTALL_DIR="/tmp/ocean/install/${OCEAN_PLATFORM}"

OTP_VALID_BUILD_CONFIGS="debug,release"
OTP_BUILD_CONFIG="${OTP_VALID_BUILD_CONFIGS}"

OTP_VALID_LINKING_TYPES="static,shared"
OTP_LINKING_TYPES="${OTP_VALID_LINKING_TYPES}"

# Collection of builds that have errors that will be listed at the end of the script
OTP_FAILED_BUILDS=()

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build Ocean:"
    echo ""
    echo "  $(basename "$0") [-h|--help] [-i|--install INSTALL_DIR] [-b|--build BUILD_DIR] [-c|--config BUILD_CONFIG]"
    echo "                   [-l|--link LINKING_TYPE]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -i | -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be installed. Otherwise builds will be installed to: ${OTP_INSTALL_DIR}"
    echo ""
    echo "  -b | -build BUILD_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be built. Otherwise builds will be installed to: ${OTP_BUILD_DIR}"
    echo ""
    echo "  -c | --config BUILD_CONFIG : The optional build configs(s) to be built; valid values are:"
    for type in $(echo "${OTP_VALID_BUILD_CONFIGS}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. If not specified, both will be built and installed."
    echo ""
    echo "  -l | -link LINKING_TYPE : The optional linking type for which will be built; valid values are:"
    for type in $(echo "${OTP_VALID_LINKING_TYPES}" | tr ',' '\n'); do
        echo "                  ${type}"
    done
    echo "                Multiple values must be separated by commas. If not specified, both will be built and installed."
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

    OTP_BUILD_DIRECTORY="${OTP_BUILD_DIR}/third-party_${LINKING_TYPE}_${BUILD_CONFIG}"
    OTP_INSTALL_DIRECTORY="${OTP_INSTALL_DIR}/${LINKING_TYPE}_${BUILD_CONFIG}"

    echo ""
    echo ""
    echo ""
    echo "Build type: ${BUILD_CONFIG}"
    echo "Linking type: ${LINKING_TYPE}"
    echo ""
    echo "Build directory: ${OTP_BUILD_DIRECTORY}"
    echo "Install directory: ${OTP_INSTALL_DIRECTORY}"
    echo ""
    echo ""
    echo ""

    eval "${OTP_SOURCE_DIR}/build_deps.sh" "${OTP_SOURCE_DIR}" "${OTP_BUILD_DIRECTORY}" ${OCEAN_PLATFORM} "-DCMAKE_INSTALL_PREFIX=${OTP_INSTALL_DIRECTORY}" "-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}" "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}"
    if [ $? != 0 ]; then
        OTP_FAILED_BUILDS+=("${LINKING_TYPE} + ${BUILD_CONFIG}")
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

echo ""

if [ "${OTP_BUILD_CONFIG}" == "" ]; then
    echo "ERROR: At least one build type has to be specified." >&2
    exit 1
fi

# Remove duplicate values
OTP_BUILD_CONFIG=$(echo "$OTP_BUILD_CONFIG" | tr ',' '\n' | sort -u)

# Only allow valid values
for type in ${OTP_BUILD_CONFIG[@]}; do
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
for type in ${OTP_LINKING_TYPES[@]}; do
    if ! echo "${OTP_VALID_LINKING_TYPES}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

echo "The third-party libraries will be build for the following combinations:"
for build_config in ${OTP_BUILD_CONFIG[@]}; do
    for link_type in ${OTP_LINKING_TYPES[@]}; do
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

for build_config in ${OTP_BUILD_CONFIG[@]}; do
    for link_type in ${OTP_LINKING_TYPES[@]}; do
        run_build "${build_config}" "${link_type}"
    done
done

if [ "${#OTP_FAILED_BUILDS[@]}" -eq 0 ]; then
    echo "All builds were successful."
else
    echo "Some builds have failed." >&2
    for config in "${OTP_FAILED_BUILDS[@]}"; do
        echo "- $config" >&2
    done
fi

if [ "${#OTP_FAILED_BUILDS[@]}" -gt 0 ]; then
    exit 1
fi
