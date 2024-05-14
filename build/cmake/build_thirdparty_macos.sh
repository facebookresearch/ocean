#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.


echo "Building the third-party libraries required for Ocean ...:"
echo " "

OCEAN_THIRD_PARTY_SOURCE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../../build/cmake/third-party && pwd )
OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY="/tmp/ocean/build/macos"
OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY="/tmp/ocean/install/macos"

# Builds the third-party libraries for Ocean (Linux & macOS)
#
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
function run_build {
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

    OCEAN_THIRD_PARTY_BUILD_DIRECTORY="${OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY}/third-party_${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_THIRD_PARTY_INSTALL_DIRECTORY="${OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY}/${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo " "
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_THIRD_PARTY_BUILD_DIRECTORY: ${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}"
    echo "OCEAN_THIRD_PARTY_INSTALL_DIRECTORY: ${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}"
    echo " "

    echo " "
    ${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}/build_deps.sh "${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}" "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" macos "-DCMAKE_INSTALL_PREFIX=${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}"
    if [ $? != 0 ]; then
        failed_configs+=("${LIBRARY_TYPE}_${BUILD_TYPE}_0")
    fi

    echo " "
    echo " "
    echo " "
}

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
