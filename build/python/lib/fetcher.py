# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Source code fetching with caching and thread safety."""

from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
import threading
import urllib.error
import urllib.request
import zipfile
from pathlib import Path
from typing import Dict, Optional

from .directories import DirectoryManager, remove_tree
from .manifest import SourceConfig
from .platform import get_android_ndk_path

# Upper bound on any single network operation. Not a performance budget — a
# full opencv clone over a slow link is legitimately slow — but a backstop so a
# stalled transfer fails with a message instead of hanging the build forever
# behind the progress display, where there is nothing to see.
DEFAULT_FETCH_TIMEOUT_SECONDS = 3600


def _fetch_timeout() -> int:
    """Network timeout in seconds, overridable via OCEAN_3P_FETCH_TIMEOUT."""
    raw = os.environ.get("OCEAN_3P_FETCH_TIMEOUT")
    if raw:
        try:
            value = int(raw)
        except ValueError:
            return DEFAULT_FETCH_TIMEOUT_SECONDS
        if value > 0:
            return value
    return DEFAULT_FETCH_TIMEOUT_SECONDS


def _extract_zip_safely(zf: zipfile.ZipFile, target_dir: Path) -> None:
    """Extract a zip archive, refusing members that escape target_dir.

    CPython's ZipFile already drops '..' components, but these archives arrive
    over the network — the ARCore .aar comes straight from dl.google.com — and
    the cost of being wrong is a write outside the build tree, so the check is
    made explicit rather than inherited. The tar paths get the equivalent from
    ``_extract_tar_safely``.
    """
    root = target_dir.resolve()
    for member in zf.infolist():
        destination = (root / member.filename).resolve()
        if destination != root and root not in destination.parents:
            raise RuntimeError(
                f"Refusing to extract '{member.filename}': it resolves outside {root}"
            )
    zf.extractall(target_dir)


def _extract_tar_safely(tf: tarfile.TarFile, target_dir: Path) -> None:
    """Extract a tar archive, refusing members that escape target_dir.

    ``extractall(filter="data")`` is the right answer but only exists from
    3.12 (and the 3.8-3.11 maintenance releases that backported it), while the
    build docs promise 3.8+. Fall back to an explicit check that covers the
    same escape routes: absolute paths, '..' traversal, and links pointing out
    of the tree.
    """
    root = target_dir.resolve()

    def _resolves_inside(name: str) -> bool:
        destination = (root / name).resolve()
        return destination == root or root in destination.parents

    for member in tf.getmembers():
        if not _resolves_inside(member.name):
            raise RuntimeError(
                f"Refusing to extract '{member.name}': it resolves outside {root}"
            )
        # A link's target is resolved at extraction time relative to the member's
        # own directory, so it needs the same check against its parent.
        if member.islnk() or member.issym():
            link_base = (root / member.name).parent
            link_target = (link_base / member.linkname).resolve()
            if link_target != root and root not in link_target.parents:
                raise RuntimeError(
                    f"Refusing to extract link '{member.name}': "
                    f"it points outside {root}"
                )

    if sys.version_info >= (3, 12):
        tf.extractall(target_dir, filter="data")
    else:
        tf.extractall(target_dir)


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
        fingerprint = self._source_fingerprint(source)

        # Check if this fetch has permanently failed
        with self._global_lock:
            if cache_key in self._failed:
                raise RuntimeError(self._failed[cache_key])

        # Fast path: already cached
        if self.dirs.source_exists(library, version, fingerprint):
            return source_dir

        # Slow path: need to fetch (with locking)
        lock = self._get_lock(library)
        with lock:
            # Check again for permanent failure (may have failed while waiting for lock)
            with self._global_lock:
                if cache_key in self._failed:
                    raise RuntimeError(self._failed[cache_key])

            # Double-check after acquiring lock
            if self.dirs.source_exists(library, version, fingerprint):
                return source_dir

            # Actually fetch
            if not quiet:
                if source_dir.exists():
                    print(
                        f"  Re-fetching {library} {version} "
                        "(source definition changed)..."
                    )
                else:
                    print(f"  Fetching {library} {version}...")
            source_dir.parent.mkdir(parents=True, exist_ok=True)

            # Clean up any partial download from previous attempt
            if source_dir.exists():
                if not quiet:
                    print("    Cleaning up partial download...")
                remove_tree(source_dir)

            try:
                if source.type == "git":
                    self._fetch_git(source, source_dir)
                elif source.type == "archive":
                    self._fetch_archive(source, source_dir)
                elif source.type == "local":
                    self._fetch_local(source, source_dir)
                elif source.type == "ndk_source":
                    self._fetch_ndk_source(source, source_dir)
                else:
                    raise ValueError(f"Unknown source type: {source.type}")

                # Apply patches and copy files after fetching
                self._apply_post_fetch(source, source_dir, quiet)

                # Mark as fetched
                self.dirs.mark_source_fetched(library, version, fingerprint)

                return source_dir
            except Exception as e:
                # Mark as permanently failed to prevent infinite retry loops
                error_msg = f"Failed to fetch {library} {version}: {e}"
                with self._global_lock:
                    self._failed[cache_key] = error_msg

                # Clean up the partial source. Never let a cleanup failure
                # replace the fetch error the user actually needs to see.
                if source_dir.exists():
                    try:
                        remove_tree(source_dir)
                    except OSError:
                        pass

                raise RuntimeError(error_msg) from e

    def _resolve_manifest_path(self, raw: str) -> Path:
        """Resolve a manifest-relative path (patch files, copy_files sources)."""
        path = Path(raw).expanduser()
        return path if path.is_absolute() else (self.manifest_dir / path).resolve()

    @staticmethod
    def _hash_path(path: Path, digest: "hashlib._Hash") -> None:
        """Fold a file's bytes, or a whole tree's, into digest."""
        if path.is_dir():
            for child in sorted(path.rglob("*")):
                if child.is_file():
                    digest.update(str(child.relative_to(path)).encode("utf-8"))
                    digest.update(child.read_bytes())
        elif path.is_file():
            digest.update(path.read_bytes())
        else:
            digest.update(b"<missing>")

    def _source_fingerprint(self, source: SourceConfig) -> str:
        """Digest everything that determines the content of the fetched tree.

        The cache directory is keyed on (library, version) alone, so without
        this a changed `ref`, an edited patch or a modified copy_files input is
        invisible: the marker exists, the fetch is skipped, and every later
        build silently compiles the old tree. That is worst for patches, which
        are edited in place while iterating on a library.
        """
        digest = hashlib.sha256()
        for field in (
            source.type,
            source.url,
            source.ref,
            source.archive_url,
            source.local_path,
            source.ndk_path,
            source.source_subdir,
            str(source.shallow),
        ):
            digest.update((field or "").encode("utf-8"))
            digest.update(b"\0")

        for src_path, dest_path in sorted((source.copy_files or {}).items()):
            digest.update(f"{src_path}->{dest_path}\0".encode("utf-8"))
            self._hash_path(self._resolve_manifest_path(src_path), digest)

        if source.patch:
            self._hash_path(self._resolve_manifest_path(source.patch), digest)

        # A `local` source is copied from inside the repository, so its content
        # is part of the definition. NDK sources are not hashed: the NDK is
        # external and already pinned by its own version.
        if source.type == "local" and source.local_path:
            self._hash_path(self._resolve_manifest_path(source.local_path), digest)

        return digest.hexdigest()

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
        result = self._run_git(
            ["fetch", "--depth", "1", "origin", commit],
            cwd=target_dir,
            check=False,
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

        # Downloaded through urlopen rather than urlretrieve so the transfer
        # carries a timeout, and inside try/finally so a failed download does
        # not leave the temporary file behind.
        with tempfile.NamedTemporaryFile(delete=False, suffix=".download") as tmp:
            archive_path = Path(tmp.name)
        try:
            try:
                with (
                    urllib.request.urlopen(
                        source.archive_url, timeout=_fetch_timeout()
                    ) as response,
                    open(archive_path, "wb") as out,
                ):
                    shutil.copyfileobj(response, out)
            except (urllib.error.URLError, OSError) as e:
                raise RuntimeError(
                    f"Failed to download {source.archive_url}: {e}"
                ) from e

            target_dir.mkdir(parents=True, exist_ok=True)

            # Extract based on extension
            url_lower = source.archive_url.lower()
            if url_lower.endswith((".zip", ".aar")):
                with zipfile.ZipFile(archive_path, "r") as zf:
                    _extract_zip_safely(zf, target_dir)
            elif url_lower.endswith((".tar.gz", ".tgz")):
                with tarfile.open(archive_path, "r:gz") as tf:
                    _extract_tar_safely(tf, target_dir)
            elif url_lower.endswith((".tar.bz2", ".tbz2")):
                with tarfile.open(archive_path, "r:bz2") as tf:
                    _extract_tar_safely(tf, target_dir)
            elif url_lower.endswith((".tar.xz", ".txz")):
                with tarfile.open(archive_path, "r:xz") as tf:
                    tf.extractall(target_dir, filter="data")
            else:
                raise ValueError(f"Unknown archive format: {source.archive_url}")

            # If archive extracts to single directory, move contents up
            self._flatten_single_dir(target_dir)
        finally:
            archive_path.unlink(missing_ok=True)

    def _fetch_local(self, source: SourceConfig, target_dir: Path) -> None:
        """Copy source from local path."""
        if not source.local_path:
            raise ValueError("Local source requires 'local_path'")

        # Resolve local_path relative to manifest directory
        local_path = Path(source.local_path)
        if not local_path.is_absolute():
            local_path = (self.manifest_dir / local_path).resolve()
        else:
            local_path = local_path.expanduser().resolve()

        if not local_path.exists():
            raise ValueError(f"Local path does not exist: {local_path}")

        shutil.copytree(local_path, target_dir)

    def _fetch_ndk_source(self, source: SourceConfig, target_dir: Path) -> None:
        """Copy source from a path within the Android NDK.

        The ndk_path field in SourceConfig specifies a path relative to the
        NDK root, e.g. "sources/android/native_app_glue".
        """
        if not source.ndk_path:
            raise ValueError("ndk_source type requires 'ndk_path'")

        ndk_root = get_android_ndk_path()
        if not ndk_root:
            raise RuntimeError(
                "Android NDK not found. Set ANDROID_NDK_HOME or ANDROID_NDK "
                "environment variable to use ndk_source libraries."
            )

        ndk_source_path = Path(ndk_root) / source.ndk_path
        if not ndk_source_path.exists():
            raise ValueError(
                f"NDK source path does not exist: {ndk_source_path}\n"
                f"  NDK root: {ndk_root}\n"
                f"  Relative path: {source.ndk_path}"
            )

        shutil.copytree(ndk_source_path, target_dir)

    def _apply_post_fetch(  # noqa: C901
        self, source: SourceConfig, target_dir: Path, quiet: bool = False
    ) -> None:
        """Apply patches and copy files after fetching source."""
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
                encoding="utf-8",
                errors="replace",
                # Neither tool may prompt: `patch` asks "File to patch:" on
                # stdin when it cannot guess, which would hang the build
                # forever behind the progress display.
                stdin=subprocess.DEVNULL,
                creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0,
            )
            if result.returncode != 0:
                git_error = (result.stderr or result.stdout or "").strip()
                # Fall back to `patch` for sources that are not git checkouts.
                # It is absent from a default Windows install, so a missing
                # tool must surface git's diagnostic, not a FileNotFoundError.
                try:
                    fallback = subprocess.run(
                        ["patch", "-p1", "-i", str(patch_path)],
                        cwd=target_dir,
                        capture_output=True,
                        text=True,
                        encoding="utf-8",
                        errors="replace",
                        stdin=subprocess.DEVNULL,
                        creationflags=(
                            subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
                        ),
                    )
                except FileNotFoundError:
                    raise RuntimeError(
                        f"Failed to apply patch {source.patch}: {git_error}"
                    ) from None
                if fallback.returncode != 0:
                    patch_error = (fallback.stderr or fallback.stdout or "").strip()
                    raise RuntimeError(
                        f"Failed to apply patch {source.patch}:\n"
                        f"  git apply: {git_error}\n"
                        f"  patch -p1: {patch_error}"
                    )

    def _flatten_single_dir(self, target_dir: Path) -> None:
        """If directory contains only one subdirectory, move its contents up."""
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
        """Run a git command.

        Raises:
            RuntimeError: If git is missing, or the command fails and check is
                set. git's stderr is included in the message — a bare
                CalledProcessError carries only the exit code, which cannot
                distinguish a missing tag from a network failure from a
                credential prompt.
        """
        cmd = ["git"] + args
        try:
            result = subprocess.run(
                cmd,
                cwd=cwd,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                timeout=_fetch_timeout(),
                creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0,
            )
        except FileNotFoundError as e:
            raise RuntimeError("git is not installed or not on PATH") from e
        except subprocess.TimeoutExpired as e:
            raise RuntimeError(
                f"git {' '.join(args)} timed out after {_fetch_timeout()}s. "
                "Set OCEAN_3P_FETCH_TIMEOUT to raise the limit."
            ) from e

        if check and result.returncode != 0:
            detail = (result.stderr or result.stdout or "").strip()
            message = f"git {' '.join(args)} failed (exit {result.returncode})"
            raise RuntimeError(f"{message}: {detail}" if detail else message)
        return result

    def get_actual_commit(self, library: str, version: str) -> Optional[str]:
        """Get the actual commit hash of a fetched source.

        Returns None for non-git sources. `git rev-parse` walks up to the
        nearest enclosing repository, so without the `.git` check an archive or
        local source unpacked underneath the user's own checkout would record
        *that* checkout's HEAD as the library's `fetched_commit`.
        """
        source_dir = self.dirs.get_source_dir(library, version)
        if not source_dir.exists() or not (source_dir / ".git").exists():
            return None

        try:
            result = subprocess.run(
                ["git", "rev-parse", "HEAD"],
                cwd=source_dir,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                check=True,
                creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0,
            )
            return result.stdout.strip()
        except subprocess.CalledProcessError:
            return None
