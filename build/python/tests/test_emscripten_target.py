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
import sys
import unittest
import unittest.mock
from pathlib import Path

_LIB_DIR = Path(__file__).resolve().parents[1] / "lib"


def _load(name: str):
    """Load a module from build/python/lib under a prefixed name.

    The prefix matters twice. It keeps lib/platform.py from shadowing the standard
    library's `platform`, which lib/platform.py itself imports. And registering the
    result in sys.modules is required, not cosmetic: dataclasses resolves a class's
    annotations through sys.modules[cls.__module__], so an unregistered module makes
    every @dataclass in it raise AttributeError on None.
    """
    registered_name = f"ocean_build_{name}"
    spec = importlib.util.spec_from_file_location(
        registered_name, _LIB_DIR / f"{name}.py"
    )
    module = importlib.util.module_from_spec(spec)
    sys.modules[registered_name] = module
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


class TestEmscriptenToolchain(unittest.TestCase):
    def setUp(self):
        self.target = platform_module.BuildTarget(
            os=platform_module.OS.EMSCRIPTEN,
            arch=platform_module.Arch.WASM32,
        )

    def test_missing_emsdk_raises_with_actionable_message(self):
        with unittest.mock.patch.dict("os.environ", {}, clear=True):
            with self.assertRaises(RuntimeError) as caught:
                platform_module.add_emscripten_options([], self.target)
        message = str(caught.exception)
        self.assertIn("EMSDK", message)
        self.assertIn("emsdk_env", message)
        self.assertIn("emscripten/emsdk", message)

    def test_toolchain_file_is_appended(self):
        with unittest.mock.patch.dict(
            "os.environ", {"EMSDK": "/opt/emsdk"}, clear=True
        ):
            cmd = []
            platform_module.add_emscripten_options(cmd, self.target)
        self.assertEqual(len(cmd), 1)
        self.assertTrue(cmd[0].startswith("-DCMAKE_TOOLCHAIN_FILE="))
        self.assertIn("Emscripten.cmake", cmd[0])
        self.assertIn("emsdk", cmd[0])

    def test_cross_compile_dispatch_reaches_emscripten(self):
        with unittest.mock.patch.dict(
            "os.environ", {"EMSDK": "/opt/emsdk"}, clear=True
        ):
            cmd = []
            platform_module.add_cross_compile_options(cmd, self.target)
        self.assertTrue(any("Emscripten.cmake" in entry for entry in cmd))

    def test_generator_is_not_visual_studio(self):
        # find_ninja_program() is patched so the test does not depend on what is
        # installed on the machine running it -- without this the assertion would go
        # red on a host with neither ninja nor make on PATH, for a reason that has
        # nothing to do with Emscripten.
        with unittest.mock.patch.object(
            platform_module, "find_ninja_program", return_value="ninja"
        ):
            generator = platform_module.get_cmake_generator(self.target)
        self.assertEqual(generator, "Ninja")


manifest_module = _load("manifest")


class TestOptInPlatforms(unittest.TestCase):
    def _library(self, platforms):
        return manifest_module.LibraryConfig.from_dict(
            "example",
            {"version": "1.0", "platforms": platforms},
        )

    def test_all_still_means_all_native_platforms(self):
        library = self._library("all")
        for native in ("macos", "ios", "linux", "android", "win"):
            self.assertTrue(
                library.supports_platform(native),
                f"{native} should be covered by 'all'",
            )

    def test_all_does_not_cover_emscripten(self):
        library = self._library("all")
        self.assertFalse(library.supports_platform("emscripten"))

    def test_explicit_mention_opts_in(self):
        library = self._library(["linux", "emscripten"])
        self.assertTrue(library.supports_platform("emscripten"))

    def test_full_target_string_is_understood(self):
        library = self._library(["emscripten"])
        self.assertTrue(library.supports_platform("emscripten_wasm32_static_release"))

    def test_all_does_not_cover_emscripten_target_string(self):
        library = self._library("all")
        self.assertFalse(library.supports_platform("emscripten_wasm32_static_release"))


if __name__ == "__main__":
    unittest.main()
