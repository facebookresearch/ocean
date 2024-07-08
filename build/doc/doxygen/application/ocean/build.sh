#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# we explicitly identify the absolute path of this script
# so that the script can be executed from any place
absolutePath="$(cd "$(dirname "$0")" && pwd)"

export OCEAN_DEVELOPMENT_PATH=${absolutePath}/../../../../../../ocean

python3 "$absolutePath"/build.nopy
