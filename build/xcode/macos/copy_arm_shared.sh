#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This script copies the shared libraries from the "-iphoneos" and "-iphonesimulator" (temporary) directories to directories with Ocean-conform names
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

export PATH_VALUE="${PLATFORM_VALUE}_${ARCHITECTURE_VALUE}_xc9_shared${RELEASE_VALUE}"

mkdir -p "$1${PATH_VALUE}"
cp -r ${CODESIGNING_FOLDER_PATH} "$1${PATH_VALUE}"
