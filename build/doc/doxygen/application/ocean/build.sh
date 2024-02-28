#!/usr/bin/env bash
# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

# we explicitly identify the absolute path of this script
# so that the script can be executed from any place
absolutePath="$(cd "$(dirname "$0")" && pwd)"

export OCEAN_DEVELOPMENT_PATH=${absolutePath}/../../../../../../ocean

python3 "$absolutePath"/build.nopy
