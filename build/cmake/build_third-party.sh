#!/usr/bin/env bash
# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

echo "Building the third-party libraries required for Ocean ...:"
echo " "

OCEAN_THIRD_PARTY_SOURCE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../../build/cmake/third-party && pwd )
OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY="/tmp"
OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY="/tmp"

# Builds the third-party libraries for Ocean (Linux & macOS)
#
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
# BUILD_PASS_INDEX: Which stage of targets to build, valid values: 0, 1
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

    BUILD_PASS_INDEX=$3
    if [[ ${BUILD_PASS_INDEX} != "0" ]] && [[ ${BUILD_PASS_INDEX} != "1" ]]; then
        echo "ERROR: Invalid value: BUILD_PASS_INDEX=${BUILD_PASS_INDEX}"
        exit 1
    fi

    OCEAN_THIRD_PARTY_BUILD_DIRECTORY="${OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY}/ocean/build/third-party_${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_THIRD_PARTY_INSTALL_DIRECTORY="${OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY}/ocean/install/${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo " "
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo "BUILD_PASS_INDEX: ${BUILD_PASS_INDEX}"
    echo " "
    echo "OCEAN_THIRD_PARTY_BUILD_DIRECTORY: ${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}"
    echo "OCEAN_THIRD_PARTY_INSTALL_DIRECTORY: ${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}"
    echo " "

    cmake -S "${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}" \
        -B "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" \
        -DCMAKE_INSTALL_PREFIX="${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DBUILD_SHARED_LIBS="${ENABLE_BUILD_SHARED_LIBS}" \
        -DBUILD_PASS_INDEX="${BUILD_PASS_INDEX}"

    cmake --build "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" --target install -- -j16

    echo " "
    echo " "
    echo " "
}

run_build Debug static 0
run_build Debug static 1

run_build Debug shared 0
run_build Debug shared 1

run_build Release static 0
run_build Release static 1

run_build Release shared 0
run_build Release shared 1
