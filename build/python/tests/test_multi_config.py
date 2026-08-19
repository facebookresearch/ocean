# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Detecting a multi-config build tree.

Multi-config generators ignore `CMAKE_BUILD_TYPE` and choose the configuration
at build time, so `cmake --build` has to be told which one to build. Getting
this wrong is silent: the build succeeds and installs whichever configuration
the generator defaults to, under a directory named after the one that was
asked for.

The cache lines below are copied verbatim from real `cmake` output on
macOS 15 with CMake 3.31 -- Xcode defines `CMAKE_CONFIGURATION_TYPES`, and
Unix Makefiles omits it entirely rather than setting it empty.

Run:
    python3 -m unittest discover -s xplat/ocean/build/python/tests
"""

from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

_MODULE_PATH = Path(__file__).resolve().parent.parent / "build_ocean.py"
_spec = importlib.util.spec_from_file_location("build_ocean", _MODULE_PATH)
build_ocean = importlib.util.module_from_spec(_spec)
sys.modules["build_ocean"] = build_ocean
_spec.loader.exec_module(build_ocean)

XCODE_CACHE = """\
CMAKE_BUILD_TYPE:STRING=Release
CMAKE_CONFIGURATION_TYPES:STRING=Debug;Release;MinSizeRel;RelWithDebInfo
CMAKE_GENERATOR:INTERNAL=Xcode
"""

MAKEFILES_CACHE = """\
CMAKE_BUILD_TYPE:STRING=Release
CMAKE_GENERATOR:INTERNAL=Unix Makefiles
"""


class TestMultiConfigDetection(unittest.TestCase):
    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)
        self.addCleanup(self._tmp.cleanup)

    def _tree(self, cache: str) -> Path:
        (self.root / "CMakeCache.txt").write_text(cache, encoding="utf-8")
        return self.root

    def test_xcode_tree_is_multi_config(self) -> None:
        self.assertTrue(
            build_ocean.uses_multi_config_generator(self._tree(XCODE_CACHE), "Xcode")
        )

    def test_makefiles_tree_is_single_config(self) -> None:
        self.assertFalse(
            build_ocean.uses_multi_config_generator(
                self._tree(MAKEFILES_CACHE), "Unix Makefiles"
            )
        )

    def test_cache_wins_over_the_generator_name(self) -> None:
        # The cache is the configured truth; the name is only a fallback. A
        # disagreement means the tree was configured with something else.
        self.assertFalse(
            build_ocean.uses_multi_config_generator(
                self._tree(MAKEFILES_CACHE), "Xcode"
            )
        )

    def test_empty_configuration_types_is_single_config(self) -> None:
        self.assertFalse(
            build_ocean.uses_multi_config_generator(
                self._tree("CMAKE_CONFIGURATION_TYPES:STRING=\n"), "Unix Makefiles"
            )
        )

    def test_missing_cache_falls_back_to_the_generator_name(self) -> None:
        absent = self.root / "never-configured"
        for generator, expected in (
            ("Xcode", True),
            ("Visual Studio 17 2022", True),
            ("Ninja Multi-Config", True),
            ("Ninja", False),
            ("Unix Makefiles", False),
        ):
            with self.subTest(generator=generator):
                self.assertEqual(
                    build_ocean.uses_multi_config_generator(absent, generator),
                    expected,
                )
