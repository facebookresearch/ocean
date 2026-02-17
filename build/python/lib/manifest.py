# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Manifest parsing and validation."""

from __future__ import annotations

import os
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional, Set

try:
    import yaml
except ImportError:
    if os.name == "nt":
        _venv_cmd = "py -3 -m venv .venv && .venv\\Scripts\\activate"
    else:
        _venv_cmd = "python3 -m venv .venv && source .venv/bin/activate"
    print(
        "Error: PyYAML is required but not installed. Install it with:\n\n"
        "  pip install pyyaml\n\n"
        "If you don't have a virtual environment set up yet, create one first:\n\n"
        f"  {_venv_cmd}\n",
        file=sys.stderr,
    )
    sys.exit(1)


@dataclass
class SourceConfig:
    """Source code location configuration."""

    type: str  # git, archive, local
    url: Optional[str] = None
    ref: Optional[str] = None  # git tag, branch, or commit
    shallow: bool = True
    archive_url: Optional[str] = None
    local_path: Optional[str] = None
    patch: Optional[str] = None  # Path to patch file (relative to manifest dir)
    copy_files: Optional[Dict[str, str]] = None  # src -> dest file mappings
    source_subdir: Optional[str] = None  # Subdirectory within source to use as root
    ndk_path: Optional[str] = None  # Subpath within Android NDK (for ndk_source type)

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "SourceConfig":
        return cls(
            type=data.get("type", "git"),
            url=data.get("url"),
            ref=data.get("ref"),
            shallow=data.get("shallow", True),
            archive_url=data.get("archive_url"),
            local_path=data.get("local_path"),
            patch=data.get("patch"),
            copy_files=data.get("copy_files"),
            source_subdir=data.get("source_subdir"),
            ndk_path=data.get("ndk_path"),
        )


@dataclass
class BuildConfig:
    """Build system configuration."""

    system: str  # cmake, autotools, meson, custom
    options: Dict[str, Any] = field(default_factory=dict)
    options_static: Dict[str, Any] = field(default_factory=dict)
    options_shared: Dict[str, Any] = field(default_factory=dict)
    options_debug: Dict[str, Any] = field(default_factory=dict)
    options_release: Dict[str, Any] = field(default_factory=dict)
    script: Optional[str] = None  # For custom builds
    cmake_targets: Optional[List[str]] = None  # Specific CMake targets to build

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "BuildConfig":
        return cls(
            system=data.get("system", "cmake"),
            options=data.get("options", {}),
            options_static=data.get("options_static", {}),
            options_shared=data.get("options_shared", {}),
            options_debug=data.get("options_debug", {}),
            options_release=data.get("options_release", {}),
            script=data.get("script"),
            cmake_targets=data.get("cmake_targets"),
        )

    def get_merged_options(self, link_type: str, config: str) -> Dict[str, Any]:
        """Get options merged with link-type and config-specific options.

        Args:
            link_type: "static" or "shared"
            config: "debug" or "release"

        Returns:
            Merged options dictionary (common + link-type + config)
        """
        merged = dict(self.options)

        # Merge link-type specific options
        if link_type == "static":
            merged.update(self.options_static)
        elif link_type == "shared":
            merged.update(self.options_shared)

        # Merge config-specific options
        if config == "debug":
            merged.update(self.options_debug)
        elif config == "release":
            merged.update(self.options_release)

        return merged


@dataclass
class LibraryConfig:
    """Configuration for a single library."""

    name: str
    version: str
    description: str
    source: SourceConfig
    build: BuildConfig
    dependencies: List[str] = field(default_factory=list)
    platforms: List[str] = field(
        default_factory=lambda: ["all"]
    )  # ["all"] or list of platform names
    link_types: List[str] = field(
        default_factory=lambda: ["all"]
    )  # ["all"] or list of link types ("static", "shared")
    optional: bool = False
    optional_group: Optional[str] = None

    @classmethod
    def from_dict(cls, name: str, data: Dict[str, Any]) -> "LibraryConfig":
        # Handle platforms as either string or list
        platforms_raw = data.get("platforms", "all")
        if isinstance(platforms_raw, str):
            if platforms_raw == "all":
                platforms = ["all"]
            else:
                platforms = [p.strip().lower() for p in platforms_raw.split(",")]
        else:
            platforms = [p.lower() for p in platforms_raw]

        # Handle link_types as either string or list
        link_types_raw = data.get("link_types", "all")
        if isinstance(link_types_raw, str):
            if link_types_raw == "all":
                link_types = ["all"]
            else:
                link_types = [lt.strip().lower() for lt in link_types_raw.split(",")]
        else:
            link_types = [lt.lower() for lt in link_types_raw]

        return cls(
            name=name,
            version=data.get("version", "unknown"),
            description=data.get("description", ""),
            source=SourceConfig.from_dict(data.get("source", {})),
            build=BuildConfig.from_dict(data.get("build", {})),
            dependencies=data.get("dependencies", []),
            platforms=platforms,
            link_types=link_types,
            optional=data.get("optional", False),
            optional_group=data.get("optional_group"),
        )

    def supports_platform(self, platform: str) -> bool:
        """Check if this library supports the given platform.

        Args:
            platform: Either an OS name (e.g., 'macos') or a full target string
                      (e.g., 'macos_arm64', 'ios_arm64_static_debug')
        """
        if "all" in self.platforms:
            return True
        # Extract just the OS part from the platform/target string
        # e.g., 'macos_arm64' -> 'macos', 'ios_arm64_static_debug' -> 'ios'
        os_part = platform.lower().split("_")[0]
        return os_part in self.platforms

    def supports_link_type(self, link_type: str) -> bool:
        """Check if this library supports the given link type.

        Args:
            link_type: Either 'static' or 'shared'
        """
        if "all" in self.link_types:
            return True
        return link_type.lower() in self.link_types


@dataclass
class ManifestDefaults:
    """Default settings from manifest."""

    shallow_clone: bool = True
    configs: List[str] = field(default_factory=lambda: ["debug", "release"])
    link_types: List[str] = field(default_factory=lambda: ["static"])

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ManifestDefaults":
        return cls(
            shallow_clone=data.get("shallow_clone", True),
            configs=data.get("configs", ["debug", "release"]),
            link_types=data.get("link_types", ["static"]),
        )


@dataclass
class Manifest:
    """Parsed manifest containing all library configurations."""

    version: int
    defaults: ManifestDefaults
    libraries: Dict[str, LibraryConfig]

    @classmethod
    def from_file(cls, path: Path) -> "Manifest":
        """Load manifest from YAML file."""
        with open(path, "r") as f:
            data = yaml.safe_load(f)
        return cls.from_dict(data)

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "Manifest":
        """Parse manifest from dictionary."""
        version = data.get("version", 1)
        defaults = ManifestDefaults.from_dict(data.get("defaults", {}))

        libraries = {}
        for name, lib_data in data.get("libraries", {}).items():
            libraries[name] = LibraryConfig.from_dict(name, lib_data)

        manifest = cls(
            version=version,
            defaults=defaults,
            libraries=libraries,
        )
        manifest.validate()
        return manifest

    def validate(self) -> None:
        """Validate the manifest for consistency."""
        errors: List[str] = []

        # Check for missing dependencies
        for name, lib in self.libraries.items():
            for dep in lib.dependencies:
                if dep not in self.libraries:
                    errors.append(
                        f"Library '{name}' depends on unknown library '{dep}'"
                    )

        # Check for circular dependencies
        try:
            self._check_circular_deps()
        except ValueError as e:
            errors.append(str(e))

        # Warn about version looking like git hash
        for name, lib in self.libraries.items():
            if self._is_git_hash(lib.version):
                print(
                    f"  WARNING: Library '{name}' has git hash as version: {lib.version}"
                )
                print("           Consider using a semantic version instead.")

        if errors:
            raise ValueError("Manifest validation failed:\n" + "\n".join(errors))

    def _check_circular_deps(self) -> None:
        """Check for circular dependencies using DFS."""
        visited: Set[str] = set()
        rec_stack: Set[str] = set()

        def dfs(name: str, path: List[str]) -> None:
            visited.add(name)
            rec_stack.add(name)

            lib = self.libraries.get(name)
            if lib:
                for dep in lib.dependencies:
                    if dep not in visited:
                        dfs(dep, path + [dep])
                    elif dep in rec_stack:
                        cycle = path[path.index(dep) :] + [dep]
                        raise ValueError(
                            f"Circular dependency detected: {' -> '.join(cycle)}"
                        )

            rec_stack.remove(name)

        for name in self.libraries:
            if name not in visited:
                dfs(name, [name])

    @staticmethod
    def _is_git_hash(version: str) -> bool:
        """Check if version looks like a git hash."""
        return len(version) == 40 and all(
            c in "0123456789abcdef" for c in version.lower()
        )

    def get_required_libraries(self) -> Dict[str, LibraryConfig]:
        """Get all non-optional libraries."""
        return {name: lib for name, lib in self.libraries.items() if not lib.optional}

    def get_optional_libraries(self) -> Dict[str, LibraryConfig]:
        """Get all optional libraries."""
        return {name: lib for name, lib in self.libraries.items() if lib.optional}

    def get_optional_groups(self) -> Dict[str, List[str]]:
        """Get optional libraries grouped by their optional_group."""
        groups: Dict[str, List[str]] = {}
        for name, lib in self.libraries.items():
            if lib.optional and lib.optional_group:
                if lib.optional_group not in groups:
                    groups[lib.optional_group] = []
                groups[lib.optional_group].append(name)
        return groups

    def filter_libraries(
        self,
        with_libs: Optional[List[str]] = None,
        with_groups: Optional[List[str]] = None,
        build_all: bool = False,
        platforms: Optional[List[str]] = None,
    ) -> Dict[str, LibraryConfig]:
        """Filter libraries based on optional flags and platform.

        Args:
            with_libs: Specific libraries to include
            with_groups: Optional groups to include
            build_all: If True, include all optional libraries
            platforms: List of platform names (e.g., ['ios', 'macos']).
                       Libraries matching ANY platform are included.
        """
        with_libs = with_libs or []
        with_groups = with_groups or []
        result: Dict[str, LibraryConfig] = {}

        for name, lib in self.libraries.items():
            # Check platform support - library must support at least one target platform
            if platforms:
                supports_any = any(lib.supports_platform(p) for p in platforms)
                if not supports_any:
                    continue

            include = False

            if not lib.optional:
                # Always include required libraries
                include = True
            elif build_all:
                # --all flag includes everything
                include = True
            elif name in with_libs:
                # Explicitly requested
                include = True
            elif lib.optional_group and lib.optional_group in with_groups:
                # Group was requested
                include = True

            if include:
                result[name] = lib
                # Also include dependencies of included libraries
                for dep in lib.dependencies:
                    if dep in self.libraries and dep not in result:
                        result[dep] = self.libraries[dep]

        return result
