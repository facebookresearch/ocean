# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Source code fetching with caching and thread safety."""

from __future__ import annotations

import subprocess
import threading
from pathlib import Path
from typing import Dict, Optional

from .directories import DirectoryManager
from .manifest import SourceConfig


class SourceFetcher:
    """Thread-safe source fetcher with caching.

    Sources are fetched once and shared across all targets.
    Uses per-library locking to allow parallel fetches of different libraries.
    """

    def __init__(
        self, dir_manager: DirectoryManager, manifest_dir: Optional[Path] = None
    ):
        self.dirs = dir_manager
        self.manifest_dir = manifest_dir or Path(__file__).parent.parent
        self._locks: Dict[str, threading.Lock] = {}
        self._global_lock = threading.Lock()
        self._failed: Dict[str, str] = {}  # Track permanently failed fetches

    def _get_lock(self, library: str) -> threading.Lock:
        """Get or create a lock for a specific library."""
        with self._global_lock:
            if library not in self._locks:
                self._locks[library] = threading.Lock()
            return self._locks[library]

    def fetch(  # noqa: C901
        self,
        library: str,
        version: str,
        source: SourceConfig,
        quiet: bool = False,
    ) -> Path:
        """Fetch source code, using cache if available.

        Thread-safe: multiple threads can call this for the same library,
        but only one will actually fetch.

        Args:
            library: Library name
            version: Library version
            source: Source configuration
            quiet: If True, suppress progress output (for TUI mode)

        Returns:
            Path to source directory

        Raises:
            RuntimeError: If fetch previously failed permanently
        """
        source_dir = self.dirs.get_source_dir(library, version)
        cache_key = f"{library}/{version}"

        # Check if this fetch has permanently failed
        with self._global_lock:
            if cache_key in self._failed:
                raise RuntimeError(self._failed[cache_key])

        # Fast path: already cached
        if self.dirs.source_exists(library, version):
            return source_dir

        # Slow path: need to fetch (with locking)
        lock = self._get_lock(library)
        with lock:
            # Check again for permanent failure (may have failed while waiting for lock)
            with self._global_lock:
                if cache_key in self._failed:
                    raise RuntimeError(self._failed[cache_key])

            # Double-check after acquiring lock
            if self.dirs.source_exists(library, version):
                return source_dir

            # Actually fetch
            if not quiet:
                print(f"  Fetching {library} {version}...")
            source_dir.parent.mkdir(parents=True, exist_ok=True)

            # Clean up any partial download from previous attempt
            if source_dir.exists():
                import shutil

                if not quiet:
                    print("    Cleaning up partial download...")
                shutil.rmtree(source_dir)

            try:
                if source.type == "git":
                    self._fetch_git(source, source_dir)
                elif source.type == "archive":
                    self._fetch_archive(source, source_dir)
                elif source.type == "local":
                    self._fetch_local(source, source_dir)
                else:
                    raise ValueError(f"Unknown source type: {source.type}")

                # Apply patches and copy files after fetching
                self._apply_post_fetch(source, source_dir, quiet)

                # Mark as fetched
                self.dirs.mark_source_fetched(library, version)

                return source_dir
            except Exception as e:
                # Mark as permanently failed to prevent infinite retry loops
                error_msg = f"Failed to fetch {library} {version}: {e}"
                with self._global_lock:
                    self._failed[cache_key] = error_msg

                # Clean up the partial source
                if source_dir.exists():
                    import shutil

                    shutil.rmtree(source_dir)

                raise RuntimeError(error_msg) from e

    def _fetch_git(self, source: SourceConfig, target_dir: Path) -> None:
        """Fetch source from git repository."""
        if not source.url:
            raise ValueError("Git source requires 'url'")
        if not source.ref:
            raise ValueError("Git source requires 'ref'")

        if source.shallow:
            if self._is_commit_hash(source.ref):
                self._shallow_clone_commit(source.url, source.ref, target_dir)
            else:
                # For tags/branches, use --depth 1 --branch
                self._run_git(
                    [
                        "clone",
                        "--depth",
                        "1",
                        "--branch",
                        source.ref,
                        "--single-branch",
                        "--recurse-submodules",
                        "--shallow-submodules",
                        source.url,
                        str(target_dir),
                    ]
                )
        else:
            # Full clone
            self._run_git(
                [
                    "clone",
                    "--recurse-submodules",
                    source.url,
                    str(target_dir),
                ]
            )
            self._run_git(["checkout", source.ref], cwd=target_dir)

    def _shallow_clone_commit(self, url: str, commit: str, target_dir: Path) -> None:
        """Shallow clone a specific commit.

        Git 2.5+ supports fetching a specific commit with --depth 1.
        For older git or servers that don't support it, we fall back to
        fetching with limited depth.
        """
        # Initialize empty repo
        self._run_git(["init", str(target_dir)])

        # Add remote
        self._run_git(["remote", "add", "origin", url], cwd=target_dir)

        # Try to fetch just the commit (git 2.5+ with server support)
        result = subprocess.run(
            ["git", "fetch", "--depth", "1", "origin", commit],
            cwd=target_dir,
            capture_output=True,
        )

        if result.returncode != 0:
            # Fallback: fetch with limited depth (note: quiet not available here)
            self._run_git(
                ["fetch", "--depth", "100", "origin"],
                cwd=target_dir,
            )

        # Checkout the commit
        self._run_git(["checkout", commit], cwd=target_dir)

        # Initialize submodules shallowly
        self._run_git(
            ["submodule", "update", "--init", "--recursive", "--depth", "1"],
            cwd=target_dir,
        )

    def _fetch_archive(self, source: SourceConfig, target_dir: Path) -> None:
        """Fetch source from archive URL."""
        if not source.archive_url:
            raise ValueError("Archive source requires 'archive_url'")

        import tarfile
        import tempfile
        import urllib.request
        import zipfile

        # Download archive (note: quiet not available here, but archive fetches are rare)
        with tempfile.NamedTemporaryFile(delete=False) as tmp:
            urllib.request.urlretrieve(source.archive_url, tmp.name)
            archive_path = Path(tmp.name)

        try:
            target_dir.mkdir(parents=True, exist_ok=True)

            # Extract based on extension
            url_lower = source.archive_url.lower()
            if url_lower.endswith(".zip"):
                with zipfile.ZipFile(archive_path, "r") as zf:
                    zf.extractall(target_dir)
            elif url_lower.endswith((".tar.gz", ".tgz")):
                with tarfile.open(archive_path, "r:gz") as tf:
                    tf.extractall(target_dir)
            elif url_lower.endswith((".tar.bz2", ".tbz2")):
                with tarfile.open(archive_path, "r:bz2") as tf:
                    tf.extractall(target_dir)
            elif url_lower.endswith((".tar.xz", ".txz")):
                with tarfile.open(archive_path, "r:xz") as tf:
                    tf.extractall(target_dir)
            else:
                raise ValueError(f"Unknown archive format: {source.archive_url}")

            # If archive extracts to single directory, move contents up
            self._flatten_single_dir(target_dir)
        finally:
            archive_path.unlink()

    def _fetch_local(self, source: SourceConfig, target_dir: Path) -> None:
        """Copy source from local path."""
        if not source.local_path:
            raise ValueError("Local source requires 'local_path'")

        import shutil

        # Resolve local_path relative to manifest directory
        local_path = Path(source.local_path)
        if not local_path.is_absolute():
            local_path = (self.manifest_dir / local_path).resolve()
        else:
            local_path = local_path.expanduser().resolve()

        if not local_path.exists():
            raise ValueError(f"Local path does not exist: {local_path}")

        shutil.copytree(local_path, target_dir)

    def _apply_post_fetch(  # noqa: C901
        self, source: SourceConfig, target_dir: Path, quiet: bool = False
    ) -> None:
        """Apply patches and copy files after fetching source."""
        import shutil

        # Determine the effective source directory (may be a subdirectory)
        effective_dir = target_dir
        if source.source_subdir:
            effective_dir = target_dir / source.source_subdir

        # Copy files first (before patching, as patches may depend on copied files)
        if source.copy_files:
            for src_path, dest_path in source.copy_files.items():
                # Resolve source path relative to manifest directory
                src = Path(src_path)
                if not src.is_absolute():
                    src = (self.manifest_dir / src).resolve()

                # Destination is relative to effective source directory
                # (respects source_subdir if specified)
                dest = effective_dir / dest_path

                # Create destination directory if needed
                dest.parent.mkdir(parents=True, exist_ok=True)

                if src.is_dir():
                    if dest.exists():
                        shutil.rmtree(dest)
                    shutil.copytree(src, dest)
                else:
                    shutil.copy2(src, dest)
                if not quiet:
                    print(f"    Copied {src_path} -> {dest_path}")

        # Apply patch if specified
        if source.patch:
            patch_path = Path(source.patch)
            if not patch_path.is_absolute():
                patch_path = (self.manifest_dir / patch_path).resolve()

            if not patch_path.exists():
                raise ValueError(f"Patch file does not exist: {patch_path}")

            if not quiet:
                print(f"    Applying patch {source.patch}...")
            result = subprocess.run(
                ["git", "apply", "--ignore-whitespace", str(patch_path)],
                cwd=target_dir,
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                # Try with patch command as fallback (for non-git sources)
                result = subprocess.run(
                    ["patch", "-p1", "-i", str(patch_path)],
                    cwd=target_dir,
                    capture_output=True,
                    text=True,
                )
                if result.returncode != 0:
                    raise RuntimeError(
                        f"Failed to apply patch {source.patch}: {result.stderr}"
                    )

    def _flatten_single_dir(self, target_dir: Path) -> None:
        """If directory contains only one subdirectory, move its contents up."""
        import shutil
        import tempfile

        entries = list(target_dir.iterdir())
        if len(entries) == 1 and entries[0].is_dir():
            single_dir = entries[0]
            # Move to temp, then move contents back
            with tempfile.TemporaryDirectory() as tmp:
                tmp_path = Path(tmp) / "content"
                shutil.move(str(single_dir), str(tmp_path))
                for item in tmp_path.iterdir():
                    shutil.move(str(item), str(target_dir / item.name))

    @staticmethod
    def _is_commit_hash(ref: str) -> bool:
        """Check if ref looks like a git commit hash.

        Recognizes both full (40-char) and short (7+ char) commit hashes.
        Short hashes are commonly used and are valid git refs.
        """
        # Commit hashes are hex strings, typically 7-40 characters
        # 7 is the minimum for short hashes, 40 is full SHA-1
        if len(ref) < 7 or len(ref) > 40:
            return False
        return all(c in "0123456789abcdefABCDEF" for c in ref)

    @staticmethod
    def _run_git(
        args: list,
        cwd: Optional[Path] = None,
        check: bool = True,
    ) -> subprocess.CompletedProcess:
        """Run a git command."""
        cmd = ["git"] + args
        return subprocess.run(
            cmd,
            cwd=cwd,
            check=check,
            capture_output=True,
            text=True,
        )

    def get_actual_commit(self, library: str, version: str) -> Optional[str]:
        """Get the actual commit hash of a fetched source."""
        source_dir = self.dirs.get_source_dir(library, version)
        if not source_dir.exists():
            return None

        try:
            result = subprocess.run(
                ["git", "rev-parse", "HEAD"],
                cwd=source_dir,
                capture_output=True,
                text=True,
                check=True,
            )
            return result.stdout.strip()
        except subprocess.CalledProcessError:
            return None
