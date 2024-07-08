#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if [[ $(uname -s) == "Darwin" ]]; then
  OCEAN_PLATFORM="macos"
elif [[ $(uname -s) == "Linux" ]]; then
  OCEAN_PLATFORM="linux"
else
  echo "ERROR: Unsupported operating system: $(uname -s)" >&2
  exit 1
fi

echo "Building Ocean ...:"
echo " "

OCEAN_SOURCE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

OCEAN_BUILD_DIR="${PWD}/ocean_build"
OCEAN_INSTALL_DIR="${PWD}/ocean_install"

OCEAN_VALID_BUILD_CONFIGS="debug,release"
OCEAN_BUILD_CONFIGS="release"

OCEAN_VALID_LINKING_TYPES="static,shared"
OCEAN_LINKING_TYPES="static"

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
    echo "                   [-l|--link LINKING_TYPE] [-t|--third-party OCEAN_THIRD_PARTY_DIR]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -i | -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be installed. Otherwise builds will be installed to: ${OCEAN_INSTALL_DIR}"
    echo ""
    echo "  -b | -build BUILD_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be built. Otherwise builds will be installed to: ${OCEAN_BUILD_DIR}"
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
    echo "  -t | --third-party : The base location where the third-party libraries of Ocean are located, if"
    echo "                they were built manually. Otherwise standard CMake locations will be search for"
    echo "                compatible third-party libraries."
    echo ""
    echo "  -h | --help : This summary"
    echo ""
}

# Builds Ocean (Linux & macOS)
#
# BUILD_CONFIG: The build type to be used, valid values: Debug, Release
# LINKING_TYPE: The linking type of libraries to be built, valid values: static, shared
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
        echo "ERROR: Invalid value: LINKING_TYPE=${LINKING_TYPE}"
        exit 1
    fi

    BUILD_DIR="${OCEAN_BUILD_DIR}/${OCEAN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"
    INSTALL_DIR="${OCEAN_INSTALL_DIR}/${OCEAN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"

    echo " "
    echo "BUILD_CONFIG: ${BUILD_CONFIG}"
    echo "LINKING_TYPE: ${LINKING_TYPE}"
    echo " "
    echo "BUILD_DIR: ${BUILD_DIR}"
    echo "INSTALL_DIR: ${INSTALL_DIR}"

    CMAKE_CONFIGURE_COMMAND="cmake \\
    -S \"${OCEAN_SOURCE_DIR}\" \\
    -B \"${BUILD_DIR}\" \\
    -DCMAKE_INSTALL_PREFIX=\"${INSTALL_DIR}\" \\
    -DCMAKE_BUILD_TYPE=\"${BUILD_CONFIG}\" \\
    -DBUILD_SHARED_LIBS=\"${ENABLE_BUILD_SHARED_LIBS}\""

    if [ -n "${OCEAN_THIRD_PARTY_DIR}" ]; then
        # This must match the INSTALL_DIR from ./build_thirdparty_linuxunix.sh
        THIRD_PARTY_DIR="${OCEAN_THIRD_PARTY_DIR}/${OCEAN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"

        echo " "
        echo "THIRD_PARTY_DIR: ${THIRD_PARTY_DIR}"
        echo " "

        CMAKE_CONFIGURE_COMMAND+="  \\
    -DCMAKE_PREFIX_PATH=\"${THIRD_PARTY_DIR}\""
    fi

    echo "CMAKE_CONFIGURE_COMMAND = ${CMAKE_CONFIGURE_COMMAND}"
    eval "${CMAKE_CONFIGURE_COMMAND}"

    cmake --build "${BUILD_DIR}" --target install -- -j16

    build_exit_code=$?

    if [ "$build_exit_code" -ne 0 ]; then
        OCEAN_FAILED_BUILDS+=("${LINKING_TYPE}_${BUILD_CONFIG}")
    fi

    echo " "
    echo " "
    echo " "

    return $build_exit_code
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

# Remove duplicate values
OCEAN_BUILD_CONFIGS=$(echo "$OCEAN_BUILD_CONFIGS" | tr ',' '\n' | sort -u)

# Only allow valid values
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

# Remove duplicate values
OCEAN_LINKING_TYPES=$(echo "$OCEAN_LINKING_TYPES" | tr ',' '\n' | sort -u)

# Only allow valid values
for type in ${OCEAN_LINKING_TYPES[@]}; do
    if ! echo "${OCEAN_VALID_LINKING_TYPES}" | grep -w "$type" > /dev/null; then
        echo "Error: Unknown build type \"${type}\"" >&2
        exit 1
    fi
done

echo "The third-party libraries will be build for the following combinations:"
for build_config in ${OCEAN_BUILD_CONFIGS[@]}; do
    for link_type in ${OCEAN_LINKING_TYPES[@]}; do
        echo " * ${build_config} + ${link_type}"
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
for build_config in ${OCEAN_BUILD_CONFIGS[@]}; do
    for link_type in ${OCEAN_LINKING_TYPES[@]}; do
        run_build "${build_config}" "${link_type}"
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
    for config in "${OTP_FAILED_BUILDS[@]}"; do
        echo "- $config" >&2
    done

    exit 1
fi
