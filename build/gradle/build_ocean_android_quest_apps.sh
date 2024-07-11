#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.


echo "Building Ocean Android apps ...:"
echo " "

if [ -z "${ANDROID_NDK}" ]; then
    echo "ERROR: Set ANDROID_NDK to the location of your Android NDK installation."
    exit 1
fi

if [ -z "${JAVA_HOME}" ]; then
    echo "ERROR: Set JAVA_HOME to the location of your Java installation."
    exit 1
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OCEAN_SOURCE_DIR=$( cd "${SCRIPT_DIR}" && cd ../.. && pwd )

OCEAN_BUILD_DIR="${PWD}/ocean_build_gradle"
OCEAN_INSTALL_DIR="${PWD}/ocean_install_gradle"

OCEAN_THIRD_PARTY_DIR=""

# Displays the supported parameters of this script
display_help()
{
    echo "Script to build Ocean Android/Quest apps:"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  $(basename "$0") [-h|--help] [-i|--install INSTALL_DIR] [-b|--build BUILD_DIR] [-c|--config BUILD_CONFIG]"
    echo "                   [-l|--link LINKING_TYPE] [-p|--abi ABI_LIST] [-s|--sdk ANDROID_SDK] [-q | --quest]"
    echo "                   [-t|--third-party OCEAN_THIRD_PARTY_DIR]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -i | -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be installed. Default installation directory: ${OCEAN_INSTALL_DIR}"
    echo ""
    echo "  -b | -build BUILD_DIR : The optional location where the third-party libraries of Ocean will"
    echo "                be built. Default build directory: ${OCEAN_BUILD_DIR}"
    echo ""
    echo "  -t | --third-party : The location where the third-party libraries of Ocean are located, if they"
    echo "                were built manually. Otherwise standard CMake locations will be search for"
    echo "                compatible third-party libraries."
    echo ""
    echo "  -h | --help : This summary"
    echo ""
}

run_builds()
{
    echo "Finding gradle projects ..."

    GRADLE_FILES=($(find "${SCRIPT_DIR}" -type f -name gradlew | sort))

    for GRADLE_FILE in ${GRADLE_FILES[@]}; do
        echo "GRADLE_FILE = ${GRADLE_FILE}"

        [ -f "${GRADLE_FILE}" ] || { echo "Error: File 'file.txt' not found." >&2; exit 1; }

        GRADLE_DIR=$(dirname ${GRADLE_FILE})

        pushd ${GRADLE_DIR} || exit

        # Build
        if [ "${OCEAN_THIRD_PARTY_DIR}" == "" ]; then
            ./gradlew assemble -PoceanGradleBuildPath="${OCEAN_BUILD_DIR}"
        else
            ./gradlew assemble -PoceanGradleBuildPath="${OCEAN_BUILD_DIR}" -PoceanThirdPartyPath="${OCEAN_THIRD_PARTY_DIR}"
        fi

        # Copy the APKs to the specified installation directory

        # Retain only application/ocean/demo/... and convert '/' with '_'
        NEW_APK_BASE_NAME="$(echo ${GRADLE_FILE} | sed 's#.*/application/ocean/##; s/\/gradlew$//; s/\//_/g; s/^/application_ocean_/')"

        if [ -f "app/build/outputs/apk/debug/app-debug.apk" ]; then
            cp "app/build/outputs/apk/debug/app-debug.apk" "${OCEAN_INSTALL_DIR}/${NEW_APK_BASE_NAME}_debug.apk"
        else
            echo "WARNING: Cannot find app/build/outputs/apk/debug/app-debug.apk"
        fi

        if [ -f "app/build/outputs/apk/release/app-release.apk" ]; then
            cp "app/build/outputs/apk/release/app-release.apk" "${OCEAN_INSTALL_DIR}/${NEW_APK_BASE_NAME}_release.apk"
        else
            echo "WARNING: Cannot find app/build/outputs/apk/release/app-release.apk"
        fi

        popd || exit

        echo " "
        echo " "
        echo " "
    done
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
    if [ "${OCEAN_THIRDPARTY_PATH}" == "" ]; then
        echo ""
        echo ""
        echo "WARNING: No location for the Ocean third-party libraries was specified (-t) and no environment variable OCEAN_THIRDPARTY_PATH exists." >&2
        echo "         If this wasn't intentional make sure to build the third-party libraries first. Please refer to the build instructions for details." >&2
        echo ""
        echo ""
    else
        echo "No location for the Ocean third-party libraries was specified (-t) but the environment variable OCEAN_THIRDPARTY_PATH exists and will be used."
    fi
else
    if [ ! -d "${OCEAN_THIRD_PARTY_DIR}" ]; then
        echo "ERROR: The following directory for the third-party libraries cannot be found: ${OCEAN_THIRD_PARTY_DIR} - did you run the script to build the third-party libraries?" >&2
        exit 1
    fi
fi

echo " "
echo " "
echo " "
echo "OCEAN_SOURCE_DIR = ${OCEAN_SOURCE_DIR}"
echo "OCEAN_BUILD_DIR = ${OCEAN_BUILD_DIR}"
echo "OCEAN_INSTALL_DIR = ${OCEAN_INSTALL_DIR}"
echo "OCEAN_THIRD_PARTY_DIR = ${OCEAN_THIRD_PARTY_DIR}"
echo " "
echo " "
echo " "

# Make sure that the install directory exists
[ -d "${OCEAN_INSTALL_DIR}" ] || mkdir -p "${OCEAN_INSTALL_DIR}"

# Build all Gradle projects
run_builds
