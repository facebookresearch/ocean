#!/usr/bin/env bash

[ ! $# -ge 3 ] &&\
echo "Usage: $0 OCEAN_THIRD_PARTY_SOURCE_DIR BUILD_DIRECTORY_BASE PLATFORM OTHER_CMAKE_ARGS" &&\
exit 1

OCEAN_THIRD_PARTY_SOURCE_DIR=$1
BUILD_DIRECTORY_BASE=$2
PLATFORM=$3

shift
shift
shift

alldeps=`cat "${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_allplatforms.txt`

platfile="${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_"${PLATFORM}".txt
if [ -f $platfile ] ; then
    alldeps="$alldeps `cat $platfile`"
fi

for dep in $alldeps; do 
    if [ ${dep:0:1} = '#' ]; then
        continue
    fi

    cmake -S "${OCEAN_THIRD_PARTY_SOURCE_DIR}" -B "${BUILD_DIRECTORY_BASE}"/$dep -DINCLUDED_DEP_NAME=$dep "$@"
    cmake --build "${BUILD_DIRECTORY_BASE}"/$dep --target install -- -j16

done

