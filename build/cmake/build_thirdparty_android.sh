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

echo "Building the third-party libraries required by Ocean for Android ...:"
echo " "

OCEAN_THIRD_PARTY_SOURCE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../../build/cmake/third-party && pwd )
OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY="/tmp"
OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY="/tmp"

# Builds the third-party libraries for Ocean (Android)
#
# ANDROID_ABI: The identifier of the Android ABI for which the build will be configured, cf. https://developer.android.com/ndk/guides/abis
# ANDROID_SDK_VERSION: The version of the Android SDK as a string, format: android-X, cf. https://developer.android.com/tools/releases/platforms#12
# BUILD_TYPE: The build type to be used, valid values: Debug, Release
# LIBRARY_TYPE: The type of libraries to be built, valid values: static, shared
function run_build_for_android {
    ANDROID_ABI=$1
    ANDROID_SDK_VERSION=$2

    BUILD_TYPE=$3
    if [[ ${BUILD_TYPE} != "Debug" ]] && [[ ${BUILD_TYPE} != "Release" ]]; then
        echo "ERROR: Invalid value: BUILD_TYPE=${BUILD_TYPE}"
        exit 1
    fi

    LIBRARY_TYPE=$4
    if [[ ${LIBRARY_TYPE} == "static" ]]; then
        ENABLE_BUILD_SHARED_LIBS="OFF"
    elif [[ ${LIBRARY_TYPE} == "shared" ]]; then
        ENABLE_BUILD_SHARED_LIBS="ON"
    else
        echo "ERROR: Invalid value: LIBRARY_TYPE=${LIBRARY_TYPE}"
        exit 1
    fi

    OCEAN_THIRD_PARTY_BUILD_DIRECTORY="${OCEAN_THIRD_PARTY_BUILD_ROOT_DIRECTORY}/ocean/build/android/third-party/${ANDROID_ABI}_${LIBRARY_TYPE}_${BUILD_TYPE}"
    OCEAN_THIRD_PARTY_INSTALL_DIRECTORY="${OCEAN_THIRD_PARTY_INSTALL_ROOT_DIRECTORY}/ocean/install/android/${ANDROID_ABI}_${LIBRARY_TYPE}_${BUILD_TYPE}"

    echo " "
    echo "ANDROID_NDK: ${ANDROID_NDK}"
    echo "JAVA_HOME: ${JAVA_HOME}"
    echo " "
    echo "ANDROID_ABI: ${ANDROID_ABI}"
    echo "ANDROID_SDK_VERSION: ${ANDROID_SDK_VERSION}"
    echo "BUILD_TYPE: ${BUILD_TYPE}"
    echo "LIBRARY_TYPE: ${LIBRARY_TYPE}"
    echo " "
    echo "OCEAN_THIRD_PARTY_BUILD_DIRECTORY: ${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}"
    echo "OCEAN_THIRD_PARTY_INSTALL_DIRECTORY: ${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}"
    echo " "

    ${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}/build_deps.sh "${OCEAN_THIRD_PARTY_SOURCE_DIRECTORY}" "${OCEAN_THIRD_PARTY_BUILD_DIRECTORY}" android \
        "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" \
        "-DANDROID_ABI=${ANDROID_ABI}" \
        "-DANDROID_PLATFORM=${ANDROID_SDK_VERSION}" \
        "-DCMAKE_ANDROID_STL=c++_static" \
        "-DCMAKE_ANDROID_NDK=${ANDROID_NDK}" \
        "-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake" \
        "-DCMAKE_INSTALL_PREFIX=${OCEAN_THIRD_PARTY_INSTALL_DIRECTORY}" \
        "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}"

    echo " "
    echo " "
    echo " "
}

run_build_for_android armeabi-v7a android-32 Debug static
run_build_for_android arm64-v8a   android-32 Debug static
run_build_for_android x86         android-32 Debug static
run_build_for_android x86_64      android-32 Debug static

run_build_for_android armeabi-v7a android-32 Debug shared
run_build_for_android arm64-v8a   android-32 Debug shared
run_build_for_android x86         android-32 Debug shared
run_build_for_android x86_64      android-32 Debug shared

run_build_for_android armeabi-v7a android-32 Release static
run_build_for_android arm64-v8a   android-32 Release static
run_build_for_android x86         android-32 Release static
run_build_for_android x86_64      android-32 Release static

run_build_for_android armeabi-v7a android-32 Release shared
run_build_for_android arm64-v8a   android-32 Release shared
run_build_for_android x86         android-32 Release shared
run_build_for_android x86_64      android-32 Release shared
