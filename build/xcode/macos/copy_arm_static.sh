#!/usr/bin/env bash
# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

# This script copies the static libraries from the "-iphoneos" and "-iphonesimulator" (temporary) directories to directories with Ocean-conform names
# First parameter: Relative path

if [ "${CONFIGURATION}" == "Release" ]; then
    export RELEASE_VALUE=""
else
    export RELEASE_VALUE="_debug"
fi

if [ "${EFFECTIVE_PLATFORM_NAME}" == "-iphoneos" ]; then
    export PLATFORM_VALUE="ios"
    export ARCHITECTURE_VALUE="arm64"
elif [ "${EFFECTIVE_PLATFORM_NAME}" == "-iphonesimulator" ]; then
    export PLATFORM_VALUE="ios"
    export ARCHITECTURE_VALUE="i386"
else
	export PLATFORM_VALUE="invalid_platform"
    export ARCHITECTURE_VALUE="invalid_architecture"
fi

export PATH_VALUE="${PLATFORM_VALUE}_${ARCHITECTURE_VALUE}_xc9_static${RELEASE_VALUE}"

mkdir -p "$1${PATH_VALUE}"
cp -r ${CODESIGNING_FOLDER_PATH} "$1${PATH_VALUE}"
