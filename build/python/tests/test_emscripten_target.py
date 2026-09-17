# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Tests for the Emscripten build target.

Emscripten is the first target Ocean supports that is not a native platform, so
two assumptions baked into the build system stop holding: that every OS in the
enum can host a build, and that "all platforms" means every OS in the enum.
These tests pin the places where that difference has to be explicit, because
both would otherwise fail silently -- the first by auto-detecting a host that
cannot exist, the second by enrolling every third-party library into a build
none of them survive.

Run with:
    python -m unittest discover -s build/python/tests
"""

from __future__ import annotations

import importlib.util
import unittest
import unittest.mock
from pathlib import Path

_LIB_DIR = Path(__file__).resolve().parents[1] / "lib"


def _load(name: str):
    spec = importlib.util.spec_from_file_location(name, _LIB_DIR / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


platform_module = _load("platform")


class TestEmscriptenEnums(unittest.TestCase):
    def test_os_enum_has_emscripten(self):
        self.assertEqual(platform_module.OS.EMSCRIPTEN.value, "emscripten")

    def test_arch_enum_has_wasm32(self):
        self.assertEqual(platform_module.Arch.WASM32.value, "wasm32")

    def test_platform_string_parses(self):
        os_val, arch_val = platform_module.parse_platform_string("emscripten_wasm32")
        self.assertEqual(os_val, platform_module.OS.EMSCRIPTEN)
        self.assertEqual(arch_val, platform_module.Arch.WASM32)

    def test_release_target_path_component(self):
        target = platform_module.BuildTarget(
            os=platform_module.OS.EMSCRIPTEN,
            arch=platform_module.Arch.WASM32,
            build_config=platform_module.BuildConfig.RELEASE,
            link_type=platform_module.LinkType.STATIC,
        )
        self.assertEqual(target.to_path_component(), "emscripten_wasm32_static")

    def test_debug_target_path_component(self):
        target = platform_module.BuildTarget(
            os=platform_module.OS.EMSCRIPTEN,
            arch=platform_module.Arch.WASM32,
            build_config=platform_module.BuildConfig.DEBUG,
            link_type=platform_module.LinkType.STATIC,
        )
        self.assertEqual(target.to_path_component(), "emscripten_wasm32_static_debug")

    def test_platform_component_is_os_only(self):
        target = platform_module.BuildTarget(
            os=platform_module.OS.EMSCRIPTEN,
            arch=platform_module.Arch.WASM32,
        )
        self.assertEqual(target.to_platform_component(), "emscripten")


if __name__ == "__main__":
    unittest.main()
