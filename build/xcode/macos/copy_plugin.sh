#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This script copies the shared librarie plugin bundle from the shared directory to the plugin directory
# First parameter: Relative path

if [ "${CONFIGURATION}" == "Release" ]; then
    export RELEASE_VALUE=""
else
    export RELEASE_VALUE="_debug"
fi

if [ "${PLATFORM_NAME}" == "macosx" ]; then
	export PLATFORM_VALUE="osx"
    export ARCHITECTURE_VALUE="x64"
elif [ "${PLATFORM_NAME}" == "iphoneos" ]; then
	export PLATFORM_VALUE="ios"
	export ARCHITECTURE_VALUE="arm64"
elif [ "${PLATFORM_NAME}" == "iphonesimulator" ]; then
	export PLATFORM_VALUE="ios"
    export ARCHITECTURE_VALUE="i386"
else
	export PLATFORM_VALUE="invalid_platform"
    export ARCHITECTURE_VALUE="invalid_architecture"
fi

export PATH_VALUE="${PLATFORM_VALUE}_${ARCHITECTURE_VALUE}_xc9_shared${RELEASE_VALUE}"

rm -r -f "$1${PATH_VALUE}/${WRAPPER_NAME}"
mkdir -p "$1${PATH_VALUE}"
cp -r ${CODESIGNING_FOLDER_PATH} "$1${PATH_VALUE}"
