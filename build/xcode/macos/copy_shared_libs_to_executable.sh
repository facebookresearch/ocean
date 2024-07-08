#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This script copies the shared libraries from the Ocean directory into the executable bundle
# First parameter: Relative path to shared libraries (e.g., ../bin/lib/)
# Parameter 2..N: library names to copy without the lib prefix, D suffix (in debug mode) or file extension (e.g., OceanBase)

if [ "${CONFIGURATION}" == "Release" ]; then
    export RELEASE_VALUE=""
    export RELEASE_SUFFIX_VALUE=""
else
    export RELEASE_VALUE="_debug"
    export RELEASE_SUFFIX_VALUE="D"
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
	echo "Copy failed: Invalid Platform!"
	exit
fi

export PATH_VALUE="$1${PLATFORM_VALUE}_${ARCHITECTURE_VALUE}_xc9_shared${RELEASE_VALUE}/"

if [ "${PLATFORM_NAME}" == "macosx" ]; then
	export SUBPATH_VALUE="Contents/MacOS/"
else
	export SUBPATH_VALUE=""
fi

for i in "${@:2}"; do
	cp -r "${PATH_VALUE}lib$i${RELEASE_SUFFIX_VALUE}.dylib" "${CODESIGNING_FOLDER_PATH}/${SUBPATH_VALUE}"
done
