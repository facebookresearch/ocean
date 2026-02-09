# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Directory management for source cache and build outputs."""

from __future__ import annotations

import json
import os
import shutil
import threading
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, Optional

from .platform import BuildTarget

# Cross-platform file locking
if os.name == "nt":
    # Windows
    import msvcrt

    def _lock_file(f):
        """Lock a file on Windows."""
        msvcrt.locking(f.fileno(), msvcrt.LK_NBLCK, 1)

    def _unlock_file(f):
        """Unlock a file on Windows."""
        msvcrt.locking(f.fileno(), msvcrt.LK_UNLCK, 1)

else:
    # Unix
    import fcntl

    def _lock_file(f):
        """Lock a file on Unix."""
        fcntl.flock(f.fileno(), fcntl.LOCK_EX)

    def _unlock_file(f):
        """Unlock a file on Unix."""
        fcntl.flock(f.fileno(), fcntl.LOCK_UN)


# Lock for thread-safe metadata file access
_metadata_lock = threading.Lock()


@dataclass
class BuildPaths:
    """Paths for a specific (library, target) build."""

    source_dir: Path  # Shared source directory
    build_dir: Path  # Build directory for this target
    install_dir: Path  # Temporary install directory
    final_dir: Path  # Final output directory


class DirectoryManager:
    """Manages directory structure for the build system.

    Directory layout:
        .ocean_3p_cache/
            sources/
                zlib/1.3.1/           # Shared source (fetched once)
                libpng/1.6.43/
            builds/
                zlib/1.3.1/
                    macos_arm64_static/       # Build dir for release
                    macos_arm64_static_debug/ # Build dir for debug
                libpng/1.6.43/
                    macos_arm64_static/
                    ...

        3rdparty/                     # Final output
            zlib/
                h/
                    macos/
                    ios/
                lib/
                    macos_arm64_static/
                    macos_arm64_static_debug/
                .build_metadata.json
            libpng/
                ...
    """

    def __init__(
        self,
        install_dir: Path,
        source_dir: Optional[Path] = None,
        build_dir: Optional[Path] = None,
    ):
        """Initialize directory manager.

        Args:
            install_dir: Root directory for final output (3rdparty/)
            source_dir: Directory for cached sources (default: .ocean_3p_cache/sources/)
            build_dir: Directory for build artifacts (default: .ocean_3p_cache/builds/)
        """
        self.install_dir = install_dir.resolve()

        # Default cache location is next to install_dir
        default_cache = self.install_dir.parent / ".ocean_3p_cache"

        # Source and build directories (can be separate or under common cache)
        self.sources_dir = (source_dir or default_cache / "sources").resolve()
        self.builds_dir = (build_dir or default_cache / "builds").resolve()

        # Create base directories
        self.sources_dir.mkdir(parents=True, exist_ok=True)
        self.builds_dir.mkdir(parents=True, exist_ok=True)
        self.install_dir.mkdir(parents=True, exist_ok=True)

    def get_source_dir(self, library: str, version: str) -> Path:
        """Get the source directory for a library (shared across all targets)."""
        return self.sources_dir / library / version

    def source_exists(self, library: str, version: str) -> bool:
        """Check if source has already been fetched."""
        source_dir = self.get_source_dir(library, version)
        # Check for a marker file that indicates successful fetch
        marker = source_dir / ".ocean_fetched"
        return marker.exists()

    def mark_source_fetched(self, library: str, version: str) -> None:
        """Mark source as successfully fetched."""
        source_dir = self.get_source_dir(library, version)
        marker = source_dir / ".ocean_fetched"
        marker.write_text(datetime.now().isoformat())

    def get_build_dir(self, library: str, version: str, target: BuildTarget) -> Path:
        """Get the build directory for a specific (library, target) combination."""
        return self.builds_dir / library / version / target.to_path_component()

    def get_install_dir(self, library: str, version: str, target: BuildTarget) -> Path:
        """Get temporary install directory for a build."""
        return self.get_build_dir(library, version, target) / "_install"

    def get_final_dir(self, library: str, version: str) -> Path:
        """Get final output directory for a library.

        Note: version parameter is kept for API compatibility and metadata,
        but is not included in the directory path.
        """
        return self.install_dir / library

    def get_final_lib_dir(
        self, library: str, version: str, target: BuildTarget
    ) -> Path:
        """Get final lib directory for a specific target."""
        return self.get_final_dir(library, version) / "lib" / target.to_path_component()

    def get_paths(self, library: str, version: str, target: BuildTarget) -> BuildPaths:
        """Get all paths for a (library, target) build."""
        return BuildPaths(
            source_dir=self.get_source_dir(library, version),
            build_dir=self.get_build_dir(library, version, target),
            install_dir=self.get_install_dir(library, version, target),
            final_dir=self.get_final_dir(library, version),
        )

    def clean_build(self, library: str, version: str, target: BuildTarget) -> None:
        """Remove build directory for a specific target."""
        build_dir = self.get_build_dir(library, version, target)
        if build_dir.exists():
            shutil.rmtree(build_dir)

    def clean_all_builds(self, library: str, version: str) -> None:
        """Remove all build directories for a library."""
        builds_base = self.builds_dir / library / version
        if builds_base.exists():
            shutil.rmtree(builds_base)

    def clean_source(self, library: str, version: str) -> None:
        """Remove source directory for a library."""
        source_dir = self.get_source_dir(library, version)
        if source_dir.exists():
            shutil.rmtree(source_dir)

    def clean_library(self, library: str, version: str) -> None:
        """Remove all cache and output for a library."""
        self.clean_source(library, version)
        self.clean_all_builds(library, version)

        # Clean final output
        final_dir = self.get_final_dir(library, version)
        if final_dir.exists():
            shutil.rmtree(final_dir)

    def clean_all(self) -> None:
        """Remove all cache directories (sources and builds)."""
        if self.sources_dir.exists():
            shutil.rmtree(self.sources_dir)
        if self.builds_dir.exists():
            shutil.rmtree(self.builds_dir)
        # Note: We don't clean install_dir by default for safety

    def write_build_metadata(
        self,
        library: str,
        version: str,
        target: BuildTarget,
        source_info: Dict,
    ) -> None:
        """Write build metadata to final output directory.

        Thread-safe: uses both threading lock and file locking to prevent
        race conditions during parallel builds.
        """
        final_dir = self.get_final_dir(library, version)
        final_dir.mkdir(parents=True, exist_ok=True)
        metadata_file = final_dir / ".build_metadata.json"

        # Use threading lock for in-process synchronization
        with _metadata_lock:
            # Use file locking for cross-process synchronization
            lock_file = final_dir / ".build_metadata.lock"
            with open(lock_file, "w") as lock_f:
                _lock_file(lock_f)
                try:
                    # Load existing metadata or create new
                    if metadata_file.exists():
                        try:
                            with open(metadata_file, "r") as f:
                                metadata = json.load(f)
                        except (json.JSONDecodeError, ValueError):
                            # File was corrupted, recreate it
                            metadata = {
                                "library": library,
                                "version": version,
                                "source": source_info,
                                "targets_built": [],
                            }
                    else:
                        metadata = {
                            "library": library,
                            "version": version,
                            "source": source_info,
                            "targets_built": [],
                        }

                    # Add this target
                    target_str = target.to_path_component()
                    if target_str not in metadata["targets_built"]:
                        metadata["targets_built"].append(target_str)
                        metadata["targets_built"].sort()

                    metadata["last_built"] = datetime.now().isoformat()

                    # Write atomically: write to temp file, then rename
                    # On Windows, we need to remove the destination first
                    temp_file = metadata_file.with_suffix(".tmp")
                    with open(temp_file, "w") as f:
                        json.dump(metadata, f, indent=2)
                    if os.name == "nt" and metadata_file.exists():
                        metadata_file.unlink()
                    temp_file.rename(metadata_file)
                finally:
                    _unlock_file(lock_f)

    def get_dependency_dirs(
        self,
        dependencies: list,
        version_map: Dict[str, str],
        target: BuildTarget,
    ) -> Dict[str, Path]:
        """Get final directories for dependencies.

        Args:
            dependencies: List of dependency library names
            version_map: Map of library name to version
            target: Build target

        Returns:
            Dict mapping dependency name to its final directory
        """
        result = {}
        for dep in dependencies:
            if dep in version_map:
                result[dep] = self.get_final_dir(dep, version_map[dep])
        return result
