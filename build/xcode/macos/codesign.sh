#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This script codesignes bundle and not bundle executables for Mac OS and iPhone (simulator and iOS)

# As the codesign function is using the timestamp for signing, a working internet connection is necessary
# In the case, no internet connection exists "--timestamp" can be removed as workaround

if [ "${PLATFORM_NAME}" == "macosx" ]; then
    if [ "${MACH_O_TYPE}" == "mh_bundle" ]; then
        codesign -s "Apple Development" --force --timestamp "${CODESIGNING_FOLDER_PATH}/Contents/MacOS/${EXECUTABLE_NAME}"
    fi
    codesign -s "Apple Development" --force --timestamp "${CODESIGNING_FOLDER_PATH}"
elif [ "${PLATFORM_NAME}" == "iphoneos" -o "${PLATFORM_NAME}" == "iphonesimulator" ]; then
    if [ "${MACH_O_TYPE}" == "mh_bundle" ]; then
        codesign -s "iPhone Developer" --force --timestamp "${CODESIGNING_FOLDER_PATH}/${EXECUTABLE_NAME}"
    fi
    codesign -s "iPhone Developer" --force --timestamp "${CODESIGNING_FOLDER_PATH}"
else
    echo "Codesign failed: Invalid Platform!"
    exit 1
fi
