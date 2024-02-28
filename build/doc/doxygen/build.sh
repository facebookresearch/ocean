#!/usr/bin/env bash
# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

# we explicitly identify the absolute path of this script
# so that the script can be executed from any place
absolutePath="$(cd "$(dirname "$0")" && pwd)"

echo Building the documentation for:

echo Ocean
cd "$absolutePath"/ocean || exit
./build.sh
cd .. || exit

echo Application Ocean
cd "$absolutePath"/application/ocean || exit
./build.sh
cd ../.. || exit

echo Ocean Interaction Java Script
cd "$absolutePath"/interaction/javascript || exit
./build.sh
cd ../.. || exit
