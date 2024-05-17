#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if [[ "$(uname)" != "Darwin" ]]; then
    echo "ERROR: This script can only be used on macOS."
    exit 1
fi

echo "Building the third-party libraries required by Ocean for iOS ...:"
echo " "

OCEAN_BUILD_SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OCEAN_THIRD_PARTY_SOURCE_DIRECTORY="${OCEAN_BUILD_SCRIPT_DIRECTORY}/third-party"
OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY="/tmp"
OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY="/tmp"

IOS_CMAKE_TOOLCHAIN_FILE="${OCEAN_BUILD_SCRIPT_DIRECTORY}/ios-cmake/ios.toolchain.cmake"

if ! [ -f "${IOS_CMAKE_TOOLCHAIN_FILE}" ]; then
  echo "ERROR: Cannot find the toolchain file that's required for iOS builds."
  exit 1
fi

# Builds the third-party libraries for Ocean (iOS)
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

    OCEAN_THIRD_PARTY_BUILD_DIRECTORY="${OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY}/ocean/build/ios/third-party/${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_THIRD_PARTY_INSTALL_DIRECTORY="${OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY}/ocean/install/ios/${LIBRARY_TYPE}_${BUILD_TYPE}"

    # The flag indicating the platform that will be built for, cf. ios.toolchain.cmake for details
    # OS64 - build for iOS (arm64 only)
    OCEAN_PLATFORM="OS64"

    echo " "
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_THIRD_PARTY_BUILD_DIRECTORY: ${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}"
    echo "OCEAN_THIRD_PARTY_INSTALL_DIRECTORY: ${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}"
    echo " "
    echo "OCEAN_PLATFORM: ${OCEAN_PLATFORM}"
    echo " "

    echo " "
    ${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}/build_deps.sh ios "${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}" "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" "-parallelizeTargets -jobs 16 CODE_SIGNING_ALLOWED=NO" \
        "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" \
        -GXcode \
        "-DCMAKE_TOOLCHAIN_FILE=${IOS_CMAKE_TOOLCHAIN_FILE}" \
        "-DPLATFORM=${OCEAN_PLATFORM}" \
        "-DCMAKE_INSTALL_PREFIX=${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}" \
        "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}" \
        "-DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER:STRING=org.ocean.thirdparty"

    # cmake --build "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" --target install -- CODE_SIGNING_ALLOWED=NO

    echo " "
    echo " "
    echo " "
}

run_build_for_ios Debug static
run_build_for_ios Debug shared

run_build_for_ios Release static
run_build_for_ios Release shared
