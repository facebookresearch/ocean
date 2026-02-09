# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Ocean Third-Party Build System - Builders."""

from builders.cmake import CMakeBuilder

__all__ = [
    "CMakeBuilder",
]
