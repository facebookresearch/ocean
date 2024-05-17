#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if [[ "$(uname)" != "Darwin" ]]; then
    echo "ERROR: This script can only be used on macOS."
    exit 1
fi

OCEAN_BUILD_SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OCEAN_SOURCE_DIRECTORY=$( cd ${OCEAN_BUILD_SCRIPT_DIRECTORY} && cd ../.. && pwd )
OCEAN_BUILD_ROOT_DIRECTORY="/tmp"
OCEAN_INSTALL_ROOT_DIRECTORY="/tmp"

IOS_CMAKE_TOOLCHAIN_FILE="${OCEAN_BUILD_SCRIPT_DIRECTORY}/ios-cmake/ios.toolchain.cmake"

# The flag indicating the platform that will be built for, cf. ios.toolchain.cmake for details
# OS64 - build for iOS (arm64 only)
OCEAN_PLATFORM="OS64"

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

# Builds Ocean for iOS with a specific build config
#
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
function run_build_for_ios {
    BUILD_TYPE=$1
    if [[ ${BUILD_TYPE} != "Debug" ]] && [[ ${BUILD_TYPE} != "Release" ]]; then
        echo "ERROR: Invalid value: BUILD_TYPE=${BUILD_TYPE}"
        exit 1
    fi

    LIBRARY_TYPE=$2
    if [[ ${LIBRARY_TYPE} == "static" ]]; then
        ENABLE_BUILD_SHARED_LIBS="OFF"
    elif [[ ${LIBRARY_TYPE} == "shared" ]]; then
        ENABLE_BUILD_SHARED_LIBS="ON"
    else
        echo "ERROR: Invalid value: LIBRARY_TYPE=${LIBRARY_TYPE}"
        exit 1
    fi

    OCEAN_BUILD_DIRECTORY="${OCEAN_BUILD_ROOT_DIRECTORY}/ocean/build/ios/${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_INSTALL_DIRECTORY="${OCEAN_INSTALL_ROOT_DIRECTORY}/ocean/install/ios/${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_BUILD_DIRECTORY: ${OCEAN_BUILD_DIRECTORY}"
    echo "OCEAN_INSTALL_DIRECTORY: ${OCEAN_INSTALL_DIRECTORY}"
    echo " "
    echo "OCEAN_PLATFORM: ${OCEAN_PLATFORM}"
    echo " "

    CMAKE_CONFIGURE_COMMAND="cmake -S\"${OCEAN_SOURCE_DIRECTORY}\" \\
    -B\"${OCEAN_BUILD_DIRECTORY}\" \\
    -DCMAKE_BUILD_TYPE=\"${BUILD_TYPE}\" \\
    -G Xcode \\
    -DCMAKE_TOOLCHAIN_FILE=\"${IOS_CMAKE_TOOLCHAIN_FILE}\" \\
    -DPLATFORM=\"${OCEAN_PLATFORM}\" \\
    -DCMAKE_INSTALL_PREFIX=\"${OCEAN_INSTALL_DIRECTORY}\" \\
    -DBUILD_SHARED_LIBS=\"${ENABLE_BUILD_SHARED_LIBS}\" \\
    -DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER:STRING=org.ocean"

    if [ -n "${THIRD_PARTY_ROOT_DIRECTORY}" ]; then
        OCEAN_THIRD_PARTY_DIRECTORY="${THIRD_PARTY_ROOT_DIRECTORY}/${LIBRARY_TYPE}_${BUILD_TYPE}"

        echo "OCEAN_THIRD_PARTY_DIRECTORY: ${OCEAN_THIRD_PARTY_DIRECTORY}"
        echo " "

        CMAKE_CONFIGURE_COMMAND+="  \\
    -DCMAKE_PREFIX_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\" \\
    -DCMAKE_MODULE_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\""
    # -DCMAKE_FIND_ROOT_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\""
    fi

    echo "CMAKE_CONFIGURE_COMMAND = ${CMAKE_CONFIGURE_COMMAND}"
    eval "${CMAKE_CONFIGURE_COMMAND}"

    cmake --build "${OCEAN_BUILD_DIRECTORY}" --target install -- CODE_SIGNING_ALLOWED=NO -parallelizeTargets -jobs 16

    echo " "
    echo " "
    echo " "
}

run_build_for_ios Debug static
run_build_for_ios Debug shared

run_build_for_ios Release static
run_build_for_ios Release shared
