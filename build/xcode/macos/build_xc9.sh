#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

echo
echo Start building OSX xc9...
echo
echo
echo

echo 1 / 8 Build Shared Debug x64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_x64_Shared_Debug" -destination 'platform=OS X,arch=x86_64' | egrep -A 5 "(error|warning):"
echo
echo

echo 2 / 8 Build Shared Release x64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_x64_Shared_Release" -destination 'platform=OS X,arch=x86_64' | egrep -A 5 "(error|warning):"
echo
echo

echo 3 / 8 Build Static Debug x64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_x64_Static_Debug" -destination 'platform=OS X,arch=x86_64' | egrep -A 5 "(error|warning):"
echo
echo

echo 4 / 8 Build Static Release x64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_x64_Static_Release" -destination 'platform=OS X,arch=x86_64' | egrep -A 5 "(error|warning):"
echo
echo

echo 5 / 8 Build Shared Debug arm64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Shared_Debug" > /dev/null
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Shared_Debug" -destination 'platform=iOS Simulator,name=iPhone 7,OS=latest' | egrep -A 5 "(error|warning):"
echo
echo

echo 6 / 8 Build Shared Release arm64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Shared_Release" > /dev/null
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Shared_Release" -destination 'platform=iOS Simulator,name=iPhone 7,OS=latest' | egrep -A 5 "(error|warning):"
echo
echo

echo 7 / 8 Build Static Debug arm64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Static_Debug" > /dev/null
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Static_Debug" -destination 'platform=iOS Simulator,name=iPhone 7,OS=latest' | egrep -A 5 "(error|warning):"
echo
echo

echo 8 / 8 Build Static Release arm64...
echo
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Static_Release" > /dev/null
xcodebuild -workspace "macos.xcworkspace" -scheme "macos_arm64_Static_Release" -destination 'platform=iOS Simulator,name=iPhone 7,OS=latest' | egrep -A 5 "(error|warning):"
echo
echo
