# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""`--emit-expected-matrix` describes the intended build, not the host.

The matrix feeds the filter-removal gate in `audit_link_variants.py`. If
generating it were allowed to narrow to what the generating machine can
compile, every platform that machine lacks would drop out of the matrix, and
the gate reading that matrix would pass without ever having covered them --
the same "absence reads as clean" failure the audit's own coverage check was
rewritten to avoid.

Run:
    python3 -m unittest discover -s xplat/ocean/build/python/tests
"""

from __future__ import annotations

import contextlib
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(_ROOT))
try:
    import build_ocean_3rdparty as builder  # noqa: E402

    HAVE_BUILDER = True
except ImportError:  # PyYAML is not installed; the rest of the suite still runs
    HAVE_BUILDER = False


@unittest.skipUnless(HAVE_BUILDER, "build_ocean_3rdparty needs PyYAML")
class TestMatrixIsHostIndependent(unittest.TestCase):
    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self.matrix = Path(self._tmp.name) / "matrix.json"
        self.addCleanup(self._tmp.cleanup)

    def _run(self, argv: list, patches: dict) -> int:
        replacements = {
            "check_toolchains": mock.Mock(side_effect=lambda t, _: (True, t)),
            "run_preflight_checks": mock.Mock(return_value=True),
            "find_unbuildable_windows_targets": mock.Mock(return_value=[]),
        }
        replacements.update(patches)
        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(sys, "argv", argv))
            stack.enter_context(mock.patch("builtins.print"))
            for name, replacement in replacements.items():
                stack.enter_context(mock.patch.object(builder, name, replacement))
            return builder.main()

    def _emit(self, targets: str, check_toolchains=None, **patches) -> list:
        argv = [
            "build_ocean_3rdparty.py",
            "--target",
            targets,
            "--emit-expected-matrix",
            str(self.matrix),
        ]
        if check_toolchains is not None:
            patches["check_toolchains"] = check_toolchains
        self.assertEqual(self._run(argv, patches), 0)
        return json.loads(self.matrix.read_text(encoding="utf-8"))["expected"]

    def test_preflight_does_not_narrow_the_matrix(self) -> None:
        # A host without a Linux toolchain must still emit the Linux rows it
        # was asked for; otherwise the gate silently stops covering Linux.
        def drop_linux(target_strs, log_level):
            return (False, [t for t in target_strs if not t.startswith("linux")])

        entries = self._emit("macos_arm64,linux_x86_64", drop_linux)
        platforms = {entry["target"].split("_")[0] for entry in entries}
        self.assertIn("linux", platforms)
        self.assertIn("macos", platforms)

    def test_toolchain_check_is_not_consulted_at_all(self) -> None:
        # Stronger than the above: the matrix is a manifest question, so the
        # host's capabilities must never enter into it.
        spy = mock.Mock(side_effect=AssertionError("toolchains must not be probed"))
        entries = self._emit("macos_arm64,linux_x86_64", spy)
        spy.assert_not_called()
        self.assertTrue(entries)

    def test_requested_targets_all_appear(self) -> None:
        def keep_all(target_strs, log_level):
            return (True, target_strs)

        entries = self._emit("macos_arm64,linux_x86_64", keep_all)
        bases = {entry["target"] for entry in entries}
        for expected in (
            "macos_arm64",
            "macos_arm64_debug",
            "linux_x86_64",
            "linux_x86_64_debug",
        ):
            self.assertIn(expected, bases)

    def test_windows_arch_pruning_does_not_narrow_the_matrix(self) -> None:
        # This prunes Windows architectures whose MSVC tools are missing on the
        # generating host. `--target win` expands to arm64, which most Visual
        # Studio installs cannot build, so letting it run here would drop
        # win_arm64 from the matrix on exactly the machines most likely to be
        # generating it.
        drop_arm64 = mock.Mock(
            return_value=[(builder.Arch.ARM64, "ARM64 tools are not installed")]
        )
        entries = self._emit("win", find_unbuildable_windows_targets=drop_arm64)
        bases = {entry["target"] for entry in entries}
        self.assertTrue(
            any(base.startswith("win_arm64") for base in bases),
            f"win_arm64 was pruned out of the matrix: {sorted(bases)}",
        )
        self.assertTrue(any(base.startswith("win_x64") for base in bases))

    def test_emitting_without_a_target_is_refused(self) -> None:
        # The default platform set is "whatever this host supports", which is
        # never a valid basis for a gate.
        argv = [
            "build_ocean_3rdparty.py",
            "--emit-expected-matrix",
            str(self.matrix),
        ]
        self.assertEqual(self._run(argv, {}), 1)
        self.assertFalse(self.matrix.exists())
