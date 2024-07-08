#!/usr/bin/env bash

[ ! $# -ge 3 ] &&\
echo "Usage: $0 PLATFORM OCEAN_THIRD_PARTY_SOURCE_DIR BUILD_DIRECTORY_BASE EXTRA_BUILD_FLAGS [OTHER_CMAKE_CONFIG_ARGS...]" &&\
exit 1

PLATFORM=$1
OCEAN_THIRD_PARTY_SOURCE_DIR=$2
BUILD_DIRECTORY_BASE=$3
EXTRA_BUILD_FLAGS=$4

shift
shift
shift
shift

alldeps=$(grep -v '^#' "${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_allplatforms.txt)

platfile="${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_"${PLATFORM}".txt
if [ -f $platfile ] ; then
    alldeps="$alldeps $(grep -v '^#' $platfile)"
fi

echo "Building the following dependencies for the platform \"${PLATFORM}\":"
current_dependency_index=1
for dep in $alldeps; do
    echo "  ${current_dependency_index}. ${dep}"
    current_dependency_index=$((current_dependency_index + 1))
done

current_dependency_index=1
for dep in $alldeps; do
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo "Building: ${current_dependency_index}. ${dep}"
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""

    cmake -S "${OCEAN_THIRD_PARTY_SOURCE_DIR}" -B "${BUILD_DIRECTORY_BASE}"/$dep -DINCLUDED_DEP_NAME=$dep "$@" \
    || exit 1

    cmake --build "${BUILD_DIRECTORY_BASE}"/$dep --target install -- $EXTRA_BUILD_FLAGS \
    || exit 2

    current_dependency_index=$((current_dependency_index + 1))
done
