# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Tests for audit_link_variants.py.

The byte-level COFF parser and the policy verdicts are the two parts that
cannot be checked by eye, and the audit's exit status is meant to gate whether
the Unix link-type filter can be removed -- so a wrong verdict here would let a
real regression land.

Run with:
    python3 -m unittest discover -s xplat/ocean/build/python/tests
"""

from __future__ import annotations

import contextlib
import importlib.util
import io
import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path

_MODULE_PATH = Path(__file__).resolve().parent.parent / "audit_link_variants.py"
_spec = importlib.util.spec_from_file_location("audit_link_variants", _MODULE_PATH)
audit = importlib.util.module_from_spec(_spec)
sys.modules["audit_link_variants"] = audit
_spec.loader.exec_module(audit)


def _member(name: str, data: bytes) -> bytes:
    """One ar archive member: 60-byte header, payload, padded to even length."""
    header = (
        name.ljust(16).encode()
        + b"0".ljust(12)
        + b"0".ljust(6)
        + b"0".ljust(6)
        + b"100666".ljust(8)
        + str(len(data)).ljust(10).encode()
        + b"`\n"
    )
    return header + data + (b"\n" if len(data) % 2 else b"")


def short_import_lib(dll: str, symbol: str = "func") -> bytes:
    """A modern import library: a short import object naming its DLL."""
    payload = (
        b"\x00\x00\xff\xff"
        + b"\x00" * 16
        + symbol.encode()
        + b"\x00"
        + dll.encode()
        + b"\x00"
    )
    return b"!<arch>\n" + _member("/", b"\x00" * 8) + _member(dll + "/", payload)


def old_style_import_lib(dll_base: str) -> bytes:
    """A pre-short-format import library, identified by its descriptor symbols."""
    symbols = (
        f"__IMPORT_DESCRIPTOR_{dll_base}".encode()
        + b"\x00"
        + b"__NULL_IMPORT_DESCRIPTOR\x00"
    )
    return (
        b"!<arch>\n"
        + _member("/", symbols)
        + _member("obj.o/", struct.pack("<HH", 0x8664, 1) + b"\x00" * 40)
    )


def static_lib() -> bytes:
    """A real static archive: COFF objects, no import records."""
    return (
        b"!<arch>\n"
        + _member("/", b"\x00" * 8)
        + _member("obj.o/", struct.pack("<HH", 0x8664, 1) + b"\x00" * 100)
    )


class TempTree(unittest.TestCase):
    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)
        self.addCleanup(self._tmp.cleanup)

    def staging(self, library: str, target: str) -> Path:
        path = self.root / library / "1.0" / target / "_install"
        path.mkdir(parents=True)
        return path

    def verdict(self, library: str, target: str, requested: str) -> str:
        staging = self.root / library / "1.0" / target / "_install"
        return audit.audit_staging(staging, library, "1.0", target, requested).verdict


class TestClassifyWindowsLib(TempTree):
    """The COFF parser. Nothing else can distinguish these two file types."""

    def _write(self, name: str, data: bytes) -> Path:
        path = self.root / name
        path.write_bytes(data)
        return path

    def test_short_import_is_import_and_names_its_dll(self) -> None:
        path = self._write("foo.lib", short_import_lib("zlib1.dll", "deflate"))
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.IMPORT, "zlib1.dll")
        )

    def test_old_style_import_is_not_reported_as_static(self) -> None:
        # Returning STATIC here would hide a missing-DLL condition, which is
        # the whole point of the audit.
        path = self._write("legacy.lib", old_style_import_lib("legacy"))
        kind, dll = audit.classify_windows_lib(path)
        self.assertEqual(kind, audit.LibKind.IMPORT)
        self.assertEqual(dll, "legacy.dll")

    def test_static_archive_is_static(self) -> None:
        path = self._write("foostatic.lib", static_lib())
        self.assertEqual(audit.classify_windows_lib(path), (audit.LibKind.STATIC, None))

    def test_not_an_archive_is_unknown(self) -> None:
        path = self._write("garbage.lib", b"this is not an archive")
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )

    def test_truncated_header_is_unknown_not_static(self) -> None:
        # STATIC here would be an assertion the file does not support, and it
        # is the answer that lets a missing DLL through.
        path = self._write("trunc.lib", b"!<arch>\nshort")
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )

    def test_truncated_payload_is_unknown(self) -> None:
        # Header claims 9999 bytes, the file holds a handful.
        header = b"x".ljust(48) + b"9999".ljust(10) + b"`\n"
        path = self._write("shortpayload.lib", b"!<arch>\n" + header + b"only a few")
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )

    def test_valid_import_then_truncated_header_is_unknown(self) -> None:
        # An import record early in the archive must not short-circuit the
        # scan: IMPORT is not a neutral verdict, because a present DLL then
        # makes the whole tree report OK.
        data = short_import_lib("foo.dll") + b"partial header"
        path = self._write("late_trunc.lib", data)
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )

    def test_valid_import_then_truncated_payload_is_unknown(self) -> None:
        header = b"x".ljust(48) + b"9999".ljust(10) + b"`\n"
        data = short_import_lib("foo.dll") + header + b"few"
        path = self._write("late_short.lib", data)
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )

    def test_valid_import_followed_by_more_good_members_is_import(self) -> None:
        # Scanning to EOF must not break the ordinary multi-member case.
        data = short_import_lib("foo.dll") + _member("bar.dll/", b"\x00" * 32)
        path = self._write("multi.lib", data)
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.IMPORT, "foo.dll")
        )

    def test_malformed_size_field_is_unknown(self) -> None:
        bad = b"!<arch>\n" + b"x".ljust(48) + b"NOTANUMBER".ljust(10) + b"`\n"
        path = self._write("bad.lib", bad)
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )


class TestPublicStem(unittest.TestCase):
    """Variant pairing. zlib is the case that motivated the whole exercise.

    These assert concrete values, not just that two names agree: a function
    that mapped everything to one constant would satisfy equality alone, and
    over-merging silently hides the dual-variant case the audit exists to find.
    """

    def test_concrete_stems(self) -> None:
        for name, expected in (
            ("zlibstatic.lib", "zlib"),
            ("zlib1.dll", "zlib"),
            ("libz.a", "z"),
            ("libz.1.3.1.dylib", "z"),
            ("libfoo.so.1.2.3", "foo"),
            ("libpng16.dll", "png"),
            ("libpng16_static.lib", "png"),
        ):
            with self.subTest(name=name):
                self.assertEqual(audit.public_stem(name), expected)

    def test_different_libraries_do_not_merge(self) -> None:
        # Over-merging would mask a real dual-variant pair.
        self.assertNotEqual(
            audit.public_stem("libssl.a"), audit.public_stem("libcrypto.a")
        )
        self.assertNotEqual(
            audit.public_stem("libfoo.a"), audit.public_stem("libbar.a")
        )

    def test_pairing_is_case_insensitive(self) -> None:
        # Windows filenames are case-insensitive and producers are not
        # consistent; comparing by exact case split one library into two groups.
        self.assertEqual(audit.public_stem("FOO.DLL"), audit.public_stem("foo.dll"))
        self.assertEqual(audit.public_stem("libFoo.A"), audit.public_stem("Foo.Lib"))

    def test_versioned_dll_pairs_with_its_static_sibling(self) -> None:
        # libpng16.dll beside libpng16_static.lib: these only pair if the
        # separator left by the marker strip is removed before the digits are.
        self.assertEqual(
            audit.public_stem("libpng16.dll"), audit.public_stem("libpng16_static.lib")
        )

    def test_name_ending_in_s_is_not_mangled(self) -> None:
        # A bare "s" variant marker used to strip the last letter, so
        # mbedtls.dll and mbedtls_static.lib landed in different groups.
        self.assertEqual(
            audit.public_stem("mbedtls.dll"), audit.public_stem("mbedtls_static.lib")
        )

    def test_library_ending_in_a_digit_still_pairs_with_itself(self) -> None:
        self.assertEqual(
            audit.public_stem("libtinyxml2.a"), audit.public_stem("libtinyxml2.dylib")
        )


class TestFrameworkRecognition(TempTree):
    """A framework binary has no extension, so nothing else identifies it."""

    def _tree(self, versioned: bool) -> Path:
        staging = self.staging("png", "macos_arm64_static")
        lib = staging / "lib"
        lib.mkdir()
        framework = lib / "libpng16.framework"
        if versioned:
            (framework / "Versions" / "A").mkdir(parents=True)
            (framework / "Versions" / "A" / "libpng16").write_text("binary")
            # The two symlinks a real bundle carries; the collector skips them,
            # so the binary must not be counted three times.
            (framework / "Versions" / "Current").symlink_to("A")
            (framework / "libpng16").symlink_to("Versions/Current/libpng16")
        else:
            framework.mkdir(parents=True)
            (framework / "libpng16").write_text("binary")
        (lib / "libpng16.a").write_text("static")
        return staging

    def test_versioned_framework_beside_static_is_dual_public(self) -> None:
        # libpng really does this on macOS; build_ocean_3rdparty.py skips
        # .framework bundles when packaging for exactly this reason.
        self._tree(versioned=True)
        finding = audit.audit_staging(
            self.root / "png" / "1.0" / "macos_arm64_static" / "_install",
            "png",
            "1.0",
            "macos_arm64_static",
            "static",
        )
        self.assertEqual(finding.verdict, "DUAL_PUBLIC")
        self.assertEqual(len(finding.artifacts), 1, finding.artifacts)
        self.assertEqual(len(finding.artifacts["png"]), 2, finding.artifacts)

    def test_flat_framework_beside_static_is_dual_public(self) -> None:
        self._tree(versioned=False)
        self.assertEqual(
            self.verdict("png", "macos_arm64_static", "static"), "DUAL_PUBLIC"
        )

    def test_unrelated_extensionless_files_are_still_ignored(self) -> None:
        # Only the binary matching the bundle name is the library. Headers,
        # resources and helper executables must stay invisible.
        staging = self.staging("thing", "macos_arm64_static")
        lib = staging / "lib"
        lib.mkdir()
        (lib / "libthing.a").write_text("static")
        (lib / "README").write_text("not a library")
        (lib / "LICENSE").write_text("not a library")
        framework = lib / "libthing.framework"
        (framework / "Versions" / "A" / "Resources").mkdir(parents=True)
        (framework / "Versions" / "A" / "Resources" / "Info").write_text("plist")
        (framework / "Versions" / "A" / "helper").write_text("not the library")
        self.assertEqual(self.verdict("thing", "macos_arm64_static", "static"), "OK")


class TestArtifactKind(unittest.TestCase):
    """Suffix classification. Nothing else tests these directly."""

    def test_unix_suffixes(self) -> None:
        for name, expected in (
            ("libfoo.a", audit.LibKind.STATIC),
            ("libfoo.so", audit.LibKind.SHARED),
            ("libfoo.so.1.2.3", audit.LibKind.SHARED),
            ("libfoo.dylib", audit.LibKind.SHARED),
            ("libfoo.1.2.dylib", audit.LibKind.SHARED),
            ("foo.dll", audit.LibKind.SHARED),
            ("README.md", None),
        ):
            with self.subTest(name=name):
                self.assertEqual(audit.artifact_kind(Path(name), False)[0], expected)

    def test_lib_suffix_is_static_off_windows(self) -> None:
        self.assertEqual(
            audit.artifact_kind(Path("foo.lib"), False)[0], audit.LibKind.STATIC
        )


class TestVerdicts(TempTree):
    """The policy. A wrong verdict here would gate the wrong decision."""

    def test_single_static_variant_is_ok(self) -> None:
        staging = self.staging("clean", "macos_arm64_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "libclean.a").write_text("s")
        self.assertEqual(self.verdict("clean", "macos_arm64_static", "static"), "OK")

    def test_both_unix_variants_is_dual_public(self) -> None:
        staging = self.staging("foo", "macos_arm64_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "libfoo.a").write_text("s")
        (staging / "lib" / "libfoo.1.2.dylib").write_text("d")
        self.assertEqual(
            self.verdict("foo", "macos_arm64_static", "static"), "DUAL_PUBLIC"
        )

    def test_static_requested_but_only_shared_installed(self) -> None:
        # The filter would delete the .so and publish nothing, so this has to
        # be fixed at the producer. Reporting OK here was a real bug.
        staging = self.staging("onlyshared", "linux_x86_64_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "libonlyshared.so").write_text("d")
        self.assertEqual(
            self.verdict("onlyshared", "linux_x86_64_static", "static"), "WRONG_ONLY"
        )

    def test_static_beside_shared_build_is_review_not_ok(self) -> None:
        staging = self.staging("helper", "linux_x86_64_shared")
        (staging / "lib").mkdir()
        (staging / "lib" / "libhelper.so").write_text("d")
        (staging / "lib" / "libhelper_util.a").write_text("s")
        self.assertEqual(
            self.verdict("helper", "linux_x86_64_shared", "shared"), "REVIEW"
        )

    def test_import_library_without_its_dll_is_broken_pair(self) -> None:
        staging = self.staging("zlib", "win_x64_vc143_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "zlib.lib").write_bytes(short_import_lib("zlib1.dll"))
        (staging / "lib" / "zlibstatic.lib").write_bytes(static_lib())
        self.assertEqual(
            self.verdict("zlib", "win_x64_vc143_static", "static"), "BROKEN_PAIR"
        )

    def test_dll_pairing_is_case_insensitive(self) -> None:
        # The import table says FOO.DLL, the installed file is foo.dll.
        # Windows does not care, and neither should the audit.
        staging = self.staging("case", "win_x64_vc143_shared")
        (staging / "lib").mkdir()
        (staging / "bin").mkdir()
        (staging / "lib" / "case.lib").write_bytes(short_import_lib("FOO.DLL"))
        (staging / "bin" / "foo.dll").write_text("d")
        finding = audit.audit_staging(
            self.root / "case" / "1.0" / "win_x64_vc143_shared" / "_install",
            "case",
            "1.0",
            "win_x64_vc143_shared",
            "shared",
        )
        self.assertEqual(finding.verdict, "OK")
        # And they must land in ONE group -- the pairing this test is named
        # after previously never happened, and the verdict was OK by accident.
        self.assertEqual(len(finding.artifacts), 1, finding.artifacts)

    def test_dangling_cmake_metadata_is_not_ok(self) -> None:
        staging = self.staging("meta", "linux_x86_64_shared")
        (staging / "lib").mkdir()
        (staging / "lib" / "libmeta.so").write_text("d")
        cmake_dir = staging / "lib" / "cmake" / "Meta"
        cmake_dir.mkdir(parents=True)
        (cmake_dir / "MetaTargets.cmake").write_text(
            "set_target_properties(Meta::Meta PROPERTIES "
            'IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libmeta_missing.so")'
        )
        self.assertEqual(
            self.verdict("meta", "linux_x86_64_shared", "shared"), "DANGLING_METADATA"
        )

    def test_unreadable_lib_is_not_reported_clean(self) -> None:
        # An archive we cannot classify says nothing about the tree. Calling it
        # "single public variant" would let the filter-removal gate pass on a
        # tree nobody has actually checked.
        staging = self.staging("mystery", "win_x64_vc143_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "mystery.lib").write_bytes(b"not an archive at all")
        self.assertEqual(
            self.verdict("mystery", "win_x64_vc143_static", "static"), "UNCLASSIFIED"
        )

    def test_unreadable_lib_beside_a_good_one_still_blocks(self) -> None:
        staging = self.staging("mixed", "win_x64_vc143_static")
        (staging / "lib").mkdir()
        (staging / "lib" / "good.lib").write_bytes(static_lib())
        (staging / "lib" / "bad.lib").write_bytes(b"!<arch>\ntruncated")
        self.assertEqual(
            self.verdict("mixed", "win_x64_vc143_static", "static"), "UNCLASSIFIED"
        )

    def test_header_only_tree_is_no_libraries(self) -> None:
        staging = self.staging("headers", "macos_arm64_static")
        (staging / "include").mkdir()
        (staging / "include" / "h.h").write_text("h")
        self.assertEqual(
            self.verdict("headers", "macos_arm64_static", "static"), "NO_LIBRARIES"
        )


class TestGating(TempTree):
    """The exit status is the gate for removing the Unix filter."""

    def _report(self, findings) -> list:
        import contextlib
        import io

        with contextlib.redirect_stdout(io.StringIO()):
            return audit.report(findings, self.root)

    def _finding(self, verdict: str, library: str = "lib") -> audit.Finding:
        return audit.Finding(
            library=library,
            version="1.0",
            target="linux_x86_64_static",
            requested="static",
            verdict=verdict,
            detail="",
        )

    def test_clean_verdicts_do_not_block(self) -> None:
        findings = [self._finding("OK", "a"), self._finding("NO_LIBRARIES", "b")]
        self.assertFalse(self._report(findings))

    def test_review_blocks(self) -> None:
        # "We could not classify this" must not read as a clean bill of health.
        self.assertTrue(self._report([self._finding("REVIEW", "r")]))

    def test_every_problem_verdict_blocks(self) -> None:
        for verdict in (
            "BROKEN_PAIR",
            "DUAL_PUBLIC",
            "WRONG_ONLY",
            "UNCLASSIFIED",
            "DANGLING_METADATA",
            "REVIEW",
        ):
            with self.subTest(verdict=verdict):
                self.assertTrue(
                    self._report([self._finding(verdict, "x")]),
                    f"{verdict} should block",
                )


class TestDiscovery(TempTree):
    def test_finds_staging_trees_and_infers_target(self) -> None:
        self.staging("foo", "macos_arm64_static")
        self.staging("bar", "win_x64_vc143_shared")
        found = {
            (lib, version, target)
            for _, lib, version, target in audit.discover(self.root)[0]
        }
        self.assertEqual(
            found,
            {
                ("foo", "1.0", "macos_arm64_static"),
                ("bar", "1.0", "win_x64_vc143_shared"),
            },
        )


class TestNegativeSize(TempTree):
    def test_negative_member_size_is_unknown(self) -> None:
        # read(-1) reads to EOF, which would swallow the rest of the archive as
        # one member and leave the short-payload guard comparing against a
        # negative number -- yielding a confident STATIC for a corrupt file.
        header = b"x".ljust(48) + b"-1".ljust(10) + b"`\n"
        path = self.root / "neg.lib"
        path.write_bytes(b"!<arch>\n" + header + short_import_lib("zlib1.dll")[8:])
        self.assertEqual(
            audit.classify_windows_lib(path), (audit.LibKind.UNKNOWN, None)
        )


class TestPairing(TempTree):
    """Two results pair only when they differ solely by link type."""

    def _finding(
        self, library: str, target: str, requested: str, version: str = "1.0"
    ) -> audit.Finding:
        return audit.Finding(
            library=library,
            version=version,
            target=target,
            requested=requested,
            verdict="OK",
            detail="",
        )

    def _unpaired(self, *findings) -> set:
        return set(audit._unpaired(list(findings)))

    def test_same_target_differing_only_by_link_type_pairs(self) -> None:
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "macos_arm64_static", "static"),
                self._finding("zlib", "macos_arm64_shared", "shared"),
            ),
            set(),
        )

    def test_one_link_type_only_is_unpaired(self) -> None:
        self.assertEqual(
            self._unpaired(self._finding("zlib", "macos_arm64_static", "static")),
            {("zlib", "1.0", "macos_arm64", "shared")},
        )

    def test_different_os_does_not_pair(self) -> None:
        # The bug this replaces: keying coverage on the library name alone let a
        # static macOS result complete the pairing for a shared Linux result, so
        # neither platform was ever actually checked.
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "macos_arm64_static", "static"),
                self._finding("zlib", "linux_x86_64_shared", "shared"),
            ),
            {
                ("zlib", "1.0", "macos_arm64", "shared"),
                ("zlib", "1.0", "linux_x86_64", "static"),
            },
        )

    def test_different_arch_does_not_pair(self) -> None:
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "macos_arm64_static", "static"),
                self._finding("zlib", "macos_x86_64_shared", "shared"),
            ),
            {
                ("zlib", "1.0", "macos_arm64", "shared"),
                ("zlib", "1.0", "macos_x86_64", "static"),
            },
        )

    def test_different_config_does_not_pair(self) -> None:
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "macos_arm64_static", "static"),
                self._finding("zlib", "macos_arm64_shared_debug", "shared"),
            ),
            {
                ("zlib", "1.0", "macos_arm64", "shared"),
                ("zlib", "1.0", "macos_arm64_debug", "static"),
            },
        )

    def test_different_version_does_not_pair(self) -> None:
        # discover() used to drop the version entirely, so a static result for
        # one version silently answered for a shared result of another.
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "macos_arm64_static", "static", version="1.3"),
                self._finding("zlib", "macos_arm64_shared", "shared", version="1.2"),
            ),
            {
                ("zlib", "1.3", "macos_arm64", "shared"),
                ("zlib", "1.2", "macos_arm64", "static"),
            },
        )

    def test_windows_toolset_stays_in_the_key(self) -> None:
        self.assertEqual(
            self._unpaired(
                self._finding("zlib", "win_x64_vc143_static", "static"),
                self._finding("zlib", "win_x64_vc142_shared", "shared"),
            ),
            {
                ("zlib", "1.0", "win_x64_vc143", "shared"),
                ("zlib", "1.0", "win_x64_vc142", "static"),
            },
        )


class TestExpectedMatrix(TempTree):
    """The gate compares against a declared matrix, never against itself."""

    def _finding(self, library: str, target: str, requested: str) -> audit.Finding:
        return audit.Finding(
            library=library,
            version="1.0",
            target=target,
            requested=requested,
            verdict="OK",
            detail="",
        )

    def _entry(self, library: str, target: str, kinds: list) -> dict:
        return {
            "library": library,
            "version": "1.0",
            "target": target,
            "link_types": kinds,
        }

    def _matrix(self, *entries) -> list:
        path = self.root / "matrix.json"
        path.write_text(json.dumps({"expected": list(entries)}), encoding="utf-8")
        return audit.load_expected_matrix(path)

    def test_absent_platform_is_still_required(self) -> None:
        # Deriving the requirement from the findings would make Windows stop
        # being required the moment nobody built it, and the run still pass.
        expected = self._matrix(
            self._entry("zlib", "macos_arm64", ["static", "shared"]),
            self._entry("zlib", "win_x64_vc143", ["static", "shared"]),
        )
        findings = [
            self._finding("zlib", "macos_arm64_static", "static"),
            self._finding("zlib", "macos_arm64_shared", "shared"),
        ]
        self.assertEqual(
            audit.check_expected_matrix(findings, expected),
            ["zlib 1.0 win_x64_vc143 (shared)", "zlib 1.0 win_x64_vc143 (static)"],
        )

    def test_static_only_producer_is_not_held_to_an_impossible_build(self) -> None:
        # mbedtls, directshow and android_native_app_glue are static-only in
        # the manifest; requiring a shared result would never be satisfiable.
        expected = self._matrix(self._entry("mbedtls", "macos_arm64", ["static"]))
        findings = [self._finding("mbedtls", "macos_arm64_static", "static")]
        self.assertEqual(audit.check_expected_matrix(findings, expected), [])

    def test_full_coverage_passes(self) -> None:
        expected = self._matrix(
            self._entry("zlib", "macos_arm64", ["static", "shared"])
        )
        findings = [
            self._finding("zlib", "macos_arm64_static", "static"),
            self._finding("zlib", "macos_arm64_shared", "shared"),
        ]
        self.assertEqual(audit.check_expected_matrix(findings, expected), [])

    def test_gate_is_not_evaluated_without_a_matrix(self) -> None:
        findings = [self._finding("zlib", "macos_arm64_static", "static")]
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            blocked = audit.report(findings, self.root)
        self.assertFalse(blocked)
        self.assertIn("NOT EVALUATED", out.getvalue())

    def test_gate_blocks_on_a_matrix_gap(self) -> None:
        findings = [self._finding("zlib", "macos_arm64_static", "static")]
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            blocked = audit.report(
                findings,
                self.root,
                ["zlib 1.0 macos_arm64 (shared)"],
                gate_evaluated=True,
            )
        self.assertTrue(blocked)
        self.assertIn("BLOCKED", out.getvalue())

    def test_gate_passes_on_a_complete_matrix(self) -> None:
        findings = [
            self._finding("zlib", "macos_arm64_static", "static"),
            self._finding("zlib", "macos_arm64_shared", "shared"),
        ]
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            blocked = audit.report(findings, self.root, [], gate_evaluated=True)
        self.assertFalse(blocked)
        self.assertIn("PASS", out.getvalue())

    def test_malformed_matrix_is_rejected(self) -> None:
        bad_inputs = (
            {},
            {"expected": []},
            {"expected": [{"library": "z"}]},
            {"expected": [self._entry("z", "t", [])]},
            {"expected": [self._entry("z", "t", ["bogus"])]},
            {"expected": ["not an object"]},
        )
        for bad in bad_inputs:
            with self.subTest(bad=bad):
                path = self.root / "bad.json"
                path.write_text(json.dumps(bad), encoding="utf-8")
                with self.assertRaises(RuntimeError):
                    audit.load_expected_matrix(path)


class TestDiscoveryPathShape(TempTree):
    """Only <library>/<version>/<target>/_install is a result."""

    def test_nested_install_is_not_mistaken_for_a_result(self) -> None:
        # A subproject's own _install would otherwise be read with its parent
        # standing in for the target, inventing a result for a target nobody
        # built -- and, before the matrix gate, one that could pair with a real
        # result and mark it covered.
        self.staging("zlib", "macos_arm64_static")
        nested = (
            self.root / "zlib" / "1.3.1" / "macos_arm64_static" / "sub" / "_install"
        )
        nested.mkdir(parents=True)
        found, unexpected = audit.discover(self.root)
        self.assertEqual(
            {(lib, version, target) for _, lib, version, target in found},
            {("zlib", "1.0", "macos_arm64_static")},
        )
        self.assertEqual(unexpected, [nested])

    def test_shallow_install_is_not_a_result(self) -> None:
        shallow = self.root / "stray" / "_install"
        shallow.mkdir(parents=True)
        found, unexpected = audit.discover(self.root)
        self.assertEqual(found, [])
        self.assertEqual(unexpected, [shallow])

    def test_well_shaped_trees_produce_no_complaints(self) -> None:
        self.staging("zlib", "macos_arm64_static")
        self.staging("zlib", "macos_arm64_shared")
        found, unexpected = audit.discover(self.root)
        self.assertEqual(len(found), 2)
        self.assertEqual(unexpected, [])
