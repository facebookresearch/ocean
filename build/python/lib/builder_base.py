# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Base class for build system adapters."""

from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, Optional, TYPE_CHECKING

from .platform import BuildTarget

if TYPE_CHECKING:
    from .preflight import LogLevel

# Type alias for progress callback: (phase: str) -> None
ProgressCallback = Callable[[str], None]


@dataclass
class BuildContext:
    """Context information for a build."""

    # Library name
    library_name: str

    # Directories
    source_dir: Path
    build_dir: Path
    install_dir: Path

    # Target configuration
    target: BuildTarget

    # Dependency directories (library name -> install prefix)
    dependency_dirs: Dict[str, Path]

    # Build options from manifest
    build_options: Dict[str, any]

    # Number of parallel jobs for compilation
    jobs: int = 0  # 0 = auto-detect

    # Log level for build output (imported lazily to avoid circular import)
    log_level: Optional["LogLevel"] = None

    # Progress callback for TUI updates: called with phase name
    progress_callback: Optional[ProgressCallback] = None

    # Log file path for build output (None = no logging)
    log_file: Optional[Path] = None

    # Visual Studio version to use on Windows (e.g., "2022", "2026")
    # None = auto-detect latest installed version
    vs_version: Optional[str] = None

    def report_progress(self, phase: str) -> None:
        """Report progress to callback if set."""
        if self.progress_callback:
            self.progress_callback(phase)

    def get_dependency_include_dir(self, dep: str) -> Optional[Path]:
        """Get include directory for a dependency.

        Uses Ocean's layout: h/<platform>/ for headers.
        """
        if dep in self.dependency_dirs:
            # Ocean layout: h/<platform>/
            platform_str = self.target.to_platform_component()
            inc = self.dependency_dirs[dep] / "h" / platform_str
            if inc.exists():
                return inc
            # Fallback to standard include/ layout
            inc = self.dependency_dirs[dep] / "include"
            if inc.exists():
                return inc
        return None

    def get_dependency_lib_dir(self, dep: str) -> Optional[Path]:
        """Get lib directory for a dependency."""
        if dep in self.dependency_dirs:
            lib_dir = (
                self.dependency_dirs[dep] / "lib" / self.target.to_path_component()
            )
            if lib_dir.exists():
                return lib_dir
        return None

    def get_dependency_cmake_prefix(self, dep: str) -> Optional[Path]:
        """Get CMake prefix path for a dependency."""
        if dep in self.dependency_dirs:
            return self.dependency_dirs[dep]
        return None


class Builder(ABC):
    """Abstract base class for build system adapters.

    Each builder implements support for a specific build system
    (CMake, Autotools, Meson, etc.) and handles the configure,
    build, and install steps.
    """

    @abstractmethod
    def configure(self, ctx: BuildContext) -> None:
        """Configure the build.

        Args:
            ctx: Build context with paths and options
        """
        pass

    @abstractmethod
    def build(self, ctx: BuildContext) -> None:
        """Build the project.

        Args:
            ctx: Build context
        """
        pass

    @abstractmethod
    def install(self, ctx: BuildContext) -> None:
        """Install the built artifacts.

        Args:
            ctx: Build context
        """
        pass

    def clean(self, ctx: BuildContext) -> None:
        """Clean build artifacts (optional).

        Args:
            ctx: Build context
        """
        import shutil

        if ctx.build_dir.exists():
            shutil.rmtree(ctx.build_dir)

    @staticmethod
    def _ensure_dir(path: Path) -> None:
        """Ensure directory exists."""
        path.mkdir(parents=True, exist_ok=True)
