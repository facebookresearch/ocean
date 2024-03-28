#!/usr/bin/env bash
# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

echo "Building Ocean ...:"
echo " "

OCEAN_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../.. && pwd )

echo "Static, release"
echo " "
INSTALL_DIRECTORY="/tmp/ocean_install_static_release"
echo "Installation directory: ${INSTALL_DIRECTORY}"
cmake -S "${OCEAN_DIRECTORY}" -B /tmp/ocean_build_static_release -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/ocean_build_static_release --target install -- -j16
echo " "
echo " "
echo " "

echo "Static, debug"
echo " "
INSTALL_DIRECTORY="/tmp/ocean_install_static_debug"
echo "Installation directory: ${INSTALL_DIRECTORY}"
cmake -S "${OCEAN_DIRECTORY}" -B /tmp/ocean_build_static_debug -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/ocean_build_static_debug --target install -- -j16
echo " "
echo " "
echo " "

echo "Shared, release"
echo " "
INSTALL_DIRECTORY="/tmp/ocean_install_shared_release"
echo "Installation directory: ${INSTALL_DIRECTORY}"
cmake -S "${OCEAN_DIRECTORY}" -B /tmp/ocean_build_shared_release -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build /tmp/ocean_build_shared_release --target install -- -j16
echo " "
echo " "
echo " "

echo "Shared, debug"
echo " "
INSTALL_DIRECTORY="/tmp/ocean_install_shared_debug"
echo "Installation directory: ${INSTALL_DIRECTORY}"
cmake -S "${OCEAN_DIRECTORY}" -B /tmp/ocean_build_shared_debug -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON
cmake --build /tmp/ocean_build_shared_debug --target install -- -j16
echo " "
echo " "
echo " "
