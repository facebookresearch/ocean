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

# OTP = OCEAN_THIRD_PARTY
OTP_SOURCE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd third-party && pwd )

OTP_BUILD_DIR="${PWD}/ocean_build_thirdparty"
OTP_INSTALL_DIR="${PWD}/ocean_install_thirdparty"

OTP_VALID_BUILD_CONFIGS="debug,release"
OTP_BUILD_CONFIG="release"

OTP_VALID_LINKING_TYPES="static,shared"
OTP_LINKING_TYPES="static"

OTP_ARCHIVE=""

# Collection of builds that have errors that will be listed at the end of the script
OTP_FAILED_BUILDS=()

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
    echo " -a | --archive ARCHIVE: If specified, this will copy the contents of INSTALL_DIR after the build"
    echo "                into a ZIP archive; the path to this archive must exist."
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

    BUILD_DIR="${OTP_BUILD_DIR}/${OCEAN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"
    INSTALL_DIR="${OTP_INSTALL_DIR}/${OCEAN_PLATFORM}_${LINKING_TYPE}_${BUILD_CONFIG}"

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

    eval "${OTP_SOURCE_DIR}/build_deps.sh" ${OCEAN_PLATFORM} "${OTP_SOURCE_DIR}" "${BUILD_DIR}" -j16 \
          "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}" \
          "-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}" \
          "-DBUILD_SHARED_LIBS=${ENABLE_BUILD_SHARED_LIBS}"
    if [ "$?" != 0 ]; then
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
        -a|--archive)
        OTP_ARCHIVE="$2"
        shift # past argument
        shift # past value
        ;;
        *)
        echo "ERROR: Unknown value \"$1\"." >&2
        exit 1
        ;;
    esac
done

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

# Copy the build artifacts into the specified archive, if applicable
if [ "${OTP_ARCHIVE}" != "" ]; then
    if [[ ${OTP_BUILD_SUCCESSFUL} == 1 && -d ${OTP_INSTALL_DIR} ]]; then
        echo "Creating \"${OTP_ARCHIVE}\" ..."
        OTP_CURRENT_DIRECTORY="${PWD}"
        cd  "${OTP_INSTALL_DIR}" && zip -rv "${OTP_ARCHIVE}" .
        cd "${OTP_CURRENT_DIRECTORY}"
        echo "done."
        echo ""
    else
        echo "WARNING: Failed to create \"${OTP_ARCHIVE}\"."
    fi
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
