#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

echo "Building Ocean ...:"
echo " "

OCEAN_SOURCE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )
OCEAN_BUILD_ROOT_DIRECTORY="/tmp/ocean/build/macos"
OCEAN_INSTALL_ROOT_DIRECTORY="/tmp/ocean/install/macos"

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

THIRD_PARTY_ROOT_DIRECTORY="${OCEAN_INSTALL_ROOT_DIRECTORY}"

if [[ $# -gt 0 ]]; then
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
        ;;
        *)
        THIRD_PARTY_ROOT_DIRECTORY="${key}"
        shift # past argument
        ;;
    esac
fi

if [ ! -d "${THIRD_PARTY_ROOT_DIRECTORY}" ]; then
    echo "ERROR: The following directory for the third-party libraries cannot be found: ${THIRD_PARTY_ROOT_DIRECTORY} - did you run the script to build the third-party libraries?"
    exit 1
fi

# Builds Ocean (Linux & macOS)
#
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
function run_build {
    BUILD_TYPE="$1"
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

    OCEAN_BUILD_DIRECTORY="${OCEAN_BUILD_ROOT_DIRECTORY}/${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_INSTALL_DIRECTORY="${OCEAN_INSTALL_ROOT_DIRECTORY}/${LIBRARY_TYPE}_${BUILD_TYPE}"

    OCEAN_THIRD_PARTY_DIRECTORY="${THIRD_PARTY_ROOT_DIRECTORY}/${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo " "
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_BUILD_DIRECTORY: ${OCEAN_BUILD_DIRECTORY}"
    echo "OCEAN_INSTALL_DIRECTORY: ${OCEAN_INSTALL_DIRECTORY}"

    CMAKE_CONFIGURE_COMMAND="cmake \\
    -S \"${OCEAN_SOURCE_DIRECTORY}\" \\
    -B \"${OCEAN_BUILD_DIRECTORY}\" \\
    -DCMAKE_INSTALL_PREFIX=\"${OCEAN_INSTALL_DIRECTORY}\" \\
    -DCMAKE_BUILD_TYPE=\"${BUILD_TYPE}\" \\
    -DBUILD_SHARED_LIBS=\"${ENABLE_BUILD_SHARED_LIBS}\""

    if [ -n "${OCEAN_THIRD_PARTY_DIRECTORY}" ]; then
        echo " "
        echo "OCEAN_THIRD_PARTY_DIRECTORY: ${OCEAN_THIRD_PARTY_DIRECTORY}"
        echo " "

        CMAKE_CONFIGURE_COMMAND+="  \\
    -DCMAKE_PREFIX_PATH=\"${OCEAN_THIRD_PARTY_DIRECTORY}\""
    fi

    echo "CMAKE_CONFIGURE_COMMAND = ${CMAKE_CONFIGURE_COMMAND}"
    eval "${CMAKE_CONFIGURE_COMMAND}"

    cmake --build "${OCEAN_BUILD_DIRECTORY}" --target install -- -j16

    build_exit_code=$?

    if [ "$build_exit_code" -ne 0 ]; then
        failed_configs+=("${LIBRARY_TYPE}_${BUILD_TYPE}")
    fi

    echo " "
    echo " "
    echo " "

    return $build_exit_code
}

failed_configs=()

run_build Debug static
run_build Debug shared

run_build Release static
run_build Release shared

if [ "${#failed_configs[@]}" -eq 0 ]; then
    echo "All builds were successful."
else
    echo "Some builds have failed." >&2
    for config in "${failed_configs[@]}"; do
        echo "- $config" >&2
    done
fi

if [ "${#failed_configs[@]}" -gt 0 ]; then
    exit 1
fi
