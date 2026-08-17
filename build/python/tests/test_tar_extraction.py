# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Tests for the tar extraction guard in lib/fetcher.py.

Archives are fetched over the network, so an escape here writes outside the
build tree. On interpreters with `tarfile.data_filter` the standard library
does the work; below that there is no safe filter and these checks are all
that stand between a malicious archive and the filesystem.

Run with:
    python3 -m unittest discover -s xplat/ocean/build/python/tests
"""

from __future__ import annotations

import io
import sys
import tarfile
import tempfile
import unittest
from pathlib import Path
from unittest import mock

_LIB = Path(__file__).resolve().parent.parent / "lib"
sys.path.insert(0, str(_LIB.parent))
from lib.fetcher import _extract_tar_safely  # noqa: E402


class TarExtractionBase(unittest.TestCase):
    def setUp(self) -> None:
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)
        self.addCleanup(self._tmp.cleanup)
        self.outside = self.root / "OUTSIDE"
        self.outside.mkdir()
        (self.outside / "victim.txt").write_text("original")
        self.dest = self.root / "dest"
        self.dest.mkdir()

    def extract(self, archive: Path, *, no_filter: bool) -> None:
        """Extract, optionally pretending the interpreter has no data filter."""
        import lib.fetcher as fetcher

        with tarfile.open(archive, "r:xz") as tf:
            if no_filter:
                with mock.patch.object(
                    fetcher, "_have_tar_data_filter", return_value=False
                ):
                    _extract_tar_safely(tf, self.dest)
            else:
                _extract_tar_safely(tf, self.dest)


class TestLinkHandling(TarExtractionBase):
    """Both link vectors, on an interpreter without the safe filter."""

    def _hardlink_archive(self) -> Path:
        # tarfile resolves a hard link's target against the extraction ROOT,
        # not the member's directory, so `a/evil -> ../OUTSIDE/x` escapes even
        # though it looks contained relative to `a/`.
        archive = self.root / "hardlink.tar.xz"
        with tarfile.open(archive, "w:xz") as tf:
            d = tarfile.TarInfo("a")
            d.type = tarfile.DIRTYPE
            tf.addfile(d)
            ti = tarfile.TarInfo("a/evil")
            ti.type = tarfile.LNKTYPE
            ti.linkname = "../OUTSIDE/victim.txt"
            tf.addfile(ti)
        return archive

    def _symlink_pivot_archive(self) -> Path:
        # An earlier symlink changes what a later path means. Validating every
        # member up front cannot see this, because the symlink does not exist
        # yet when the later path is checked.
        archive = self.root / "pivot.tar.xz"
        with tarfile.open(archive, "w:xz") as tf:
            ln = tarfile.TarInfo("link")
            ln.type = tarfile.SYMTYPE
            ln.linkname = str(self.outside)
            tf.addfile(ln)
            payload = b"PWNED"
            f = tarfile.TarInfo("link/escaped.txt")
            f.size = len(payload)
            tf.addfile(f, io.BytesIO(payload))
        return archive

    def test_hardlink_refused_without_data_filter(self) -> None:
        with self.assertRaises(RuntimeError):
            self.extract(self._hardlink_archive(), no_filter=True)
        self.assertEqual(
            (self.outside / "victim.txt").read_text(),
            "original",
            "the file outside the destination must be untouched",
        )

    def test_symlink_pivot_refused_without_data_filter(self) -> None:
        with self.assertRaises(RuntimeError):
            self.extract(self._symlink_pivot_archive(), no_filter=True)
        self.assertFalse(
            (self.outside / "escaped.txt").exists(),
            "nothing may be written outside the destination",
        )

    def test_special_files_refused_without_data_filter(self) -> None:
        # A FIFO has no place in a source tree and hangs the next build step
        # that opens it. Device nodes are equally out of place. `data_filter`
        # rejects all of these, so the fallback must too -- which is why the
        # check is an allowlist of files and directories rather than a
        # denylist of link types.
        for name, kind in (
            ("pipe", tarfile.FIFOTYPE),
            ("chardev", tarfile.CHRTYPE),
            ("blockdev", tarfile.BLKTYPE),
        ):
            with self.subTest(kind=name):
                archive = self.root / f"{name}.tar.xz"
                with tarfile.open(archive, "w:xz") as tf:
                    ti = tarfile.TarInfo(name)
                    ti.type = kind
                    ti.mode = 0o644
                    if kind in (tarfile.CHRTYPE, tarfile.BLKTYPE):
                        ti.devmajor, ti.devminor = 1, 3
                    tf.addfile(ti)
                dest = self.root / f"dest_{name}"
                dest.mkdir()
                saved, self.dest = self.dest, dest
                try:
                    with self.assertRaises(RuntimeError):
                        self.extract(archive, no_filter=True)
                    self.assertFalse(
                        (dest / name).exists(),
                        f"no {name} may be created in the destination",
                    )
                finally:
                    self.dest = saved

    def test_plain_symlink_also_refused_without_data_filter(self) -> None:
        # Even a contained link is refused: validating links without the
        # standard filter cannot be done safely, so none are accepted.
        archive = self.root / "inner.tar.xz"
        with tarfile.open(archive, "w:xz") as tf:
            ln = tarfile.TarInfo("link")
            ln.type = tarfile.SYMTYPE
            ln.linkname = "target.txt"
            tf.addfile(ln)
        with self.assertRaises(RuntimeError):
            self.extract(archive, no_filter=True)


class TestPathTraversal(TarExtractionBase):
    """Path checks apply on every interpreter."""

    def _traversal_archive(self) -> Path:
        archive = self.root / "traversal.tar.xz"
        with tarfile.open(archive, "w:xz") as tf:
            payload = b"pwned"
            ti = tarfile.TarInfo("../escaped.txt")
            ti.size = len(payload)
            tf.addfile(ti, io.BytesIO(payload))
        return archive

    def test_traversal_refused_with_filter(self) -> None:
        with self.assertRaises(RuntimeError):
            self.extract(self._traversal_archive(), no_filter=False)

    def test_traversal_refused_without_filter(self) -> None:
        with self.assertRaises(RuntimeError):
            self.extract(self._traversal_archive(), no_filter=True)
        self.assertFalse((self.root / "escaped.txt").exists())

    def test_ordinary_archive_extracts(self) -> None:
        archive = self.root / "good.tar.xz"
        payload = self.root / "payload"
        payload.mkdir()
        (payload / "a.txt").write_text("hi")
        with tarfile.open(archive, "w:xz") as tf:
            tf.add(payload, arcname="payload")
        self.extract(archive, no_filter=False)
        self.assertEqual((self.dest / "payload" / "a.txt").read_text(), "hi")

    def test_ordinary_archive_extracts_without_filter(self) -> None:
        archive = self.root / "good2.tar.xz"
        payload = self.root / "payload2"
        payload.mkdir()
        (payload / "b.txt").write_text("ok")
        with tarfile.open(archive, "w:xz") as tf:
            tf.add(payload, arcname="payload2")
        self.extract(archive, no_filter=True)
        self.assertEqual((self.dest / "payload2" / "b.txt").read_text(), "ok")
