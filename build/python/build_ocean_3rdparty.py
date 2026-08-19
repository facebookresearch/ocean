#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

# @nolint

"""Ocean Third-Party Build System - Main Orchestrator.

This script builds third-party libraries for Ocean with:
- Declarative YAML manifest for dependencies
- DAG-based parallel builds
- Cross-platform support (macOS, iOS, Linux, Android, Windows)
- Shared source cache (fetch once, build many)
- Debug and release configurations
- Static and shared linking
- Build statistics with timing information

Usage:
    # Build all required libraries for all supported platforms (default)
    ./build_ocean_3rdparty.py

    # Build for a specific target platform
    ./build_ocean_3rdparty.py --target ios_arm64

    # Build for an OS group (all architectures for that OS)
    ./build_ocean_3rdparty.py --target android
    ./build_ocean_3rdparty.py --target macos

    # Build for multiple platforms or groups (both styles work)
    ./build_ocean_3rdparty.py --target ios --target macos
    ./build_ocean_3rdparty.py --target android,ios
    ./build_ocean_3rdparty.py --target android,macos_arm64

    # Build release only
    ./build_ocean_3rdparty.py --config release

    # Build shared libraries
    ./build_ocean_3rdparty.py --link shared

    # Build both static and shared (both styles work)
    ./build_ocean_3rdparty.py --link static --link shared
    ./build_ocean_3rdparty.py --link static,shared

    # Combining options
    ./build_ocean_3rdparty.py -t ios,macos -l static,shared -c release

    # Include optional libraries
    ./build_ocean_3rdparty.py --with opencv --with openssl

    # Build specific library and its dependencies
    ./build_ocean_3rdparty.py --library libpng
    ./build_ocean_3rdparty.py -L libpng,freetype

    # Rebuild a single library (dependencies already built)
    ./build_ocean_3rdparty.py --library curl --target ios

    # Show build plan without building
    ./build_ocean_3rdparty.py --dry-run

    # Clean and rebuild
    ./build_ocean_3rdparty.py --clean
"""

from __future__ import annotations

import argparse
import atexit
import json
import os
import sys
import time
from concurrent.futures import as_completed, ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, NamedTuple, Optional, Set

# Windows console mode restoration
# When printing Unicode characters on Windows, Python may enable Virtual Terminal
# Processing mode. This can persist after the script exits, causing arrow keys
# to display as escape sequences (e.g., ^[[A). We save and restore the console
# mode for both stdin (input) and stdout (output) to prevent this.
_windows_stdin_mode_saved = None
_windows_stdout_mode_saved = None

if os.name == "nt":  # noqa: C901
    try:
        import ctypes
        from ctypes import wintypes

        _kernel32 = ctypes.windll.kernel32
        _STD_INPUT_HANDLE = -10
        _STD_OUTPUT_HANDLE = -11

        def _save_windows_console_mode() -> None:
            """Save the current Windows console input and output modes."""
            global _windows_stdin_mode_saved, _windows_stdout_mode_saved
            try:
                # Save stdin mode (affects keyboard input like arrow keys)
                stdin_handle = _kernel32.GetStdHandle(_STD_INPUT_HANDLE)
                stdin_mode = wintypes.DWORD()
                if _kernel32.GetConsoleMode(stdin_handle, ctypes.byref(stdin_mode)):
                    _windows_stdin_mode_saved = stdin_mode.value

                # Save stdout mode (affects output processing)
                stdout_handle = _kernel32.GetStdHandle(_STD_OUTPUT_HANDLE)
                stdout_mode = wintypes.DWORD()
                if _kernel32.GetConsoleMode(stdout_handle, ctypes.byref(stdout_mode)):
                    _windows_stdout_mode_saved = stdout_mode.value
            except Exception:
                pass

        def _restore_windows_console_mode() -> None:
            """Restore the saved Windows console input and output modes."""
            try:
                # Restore stdin mode
                if _windows_stdin_mode_saved is not None:
                    stdin_handle = _kernel32.GetStdHandle(_STD_INPUT_HANDLE)
                    _kernel32.SetConsoleMode(stdin_handle, _windows_stdin_mode_saved)

                # Restore stdout mode
                if _windows_stdout_mode_saved is not None:
                    stdout_handle = _kernel32.GetStdHandle(_STD_OUTPUT_HANDLE)
                    _kernel32.SetConsoleMode(stdout_handle, _windows_stdout_mode_saved)
            except Exception:
                pass

        _save_windows_console_mode()
        atexit.register(_restore_windows_console_mode)
    except Exception:
        pass

# Add lib to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from builders.cmake import CMakeBuilder
from builders.header_only import HeaderOnlyBuilder
from builders.imported_shared import ImportedSharedBuilder
from lib import (
    Arch,
    BuildConfig,
    BuildTarget,
    check_toolchains,
    DependencyGraph,
    DirectoryManager,
    LibraryConfig,
    LinkType,
    LogLevel,
    Manifest,
    OS,
    parse_platform_string,
    print_build_plan,
    run_preflight_checks,
    SourceFetcher,
)
from lib.artifact_shapes import target_without_link_type
from lib.builder_base import BuildContext, Builder
from lib.directories import remove_tree
from lib.platform import (
    configure_console_encoding,
    DEFAULT_ANDROID_API_LEVEL,
    detect_host_os,
    detect_windows_archs,
    get_all_installed_vs_versions,
    get_msvc_toolset_version_and_path,
    WINDOWS_ARCH_COMPONENTS,
)
from lib.progress import BuildPhase, ProgressDisplay


# ============================================================================
# Defaults
# ============================================================================

# Default output directories (relative to current working directory)
DEFAULT_BASE_DIR = Path("ocean_3rdparty")
DEFAULT_BUILD_DIR = DEFAULT_BASE_DIR / "build"
DEFAULT_INSTALL_DIR = DEFAULT_BASE_DIR / "install"
DEFAULT_SOURCE_DIR = DEFAULT_BASE_DIR / "source"

# Platform group shortcuts: map OS group name to all (OS, Arch) tuples
PLATFORM_GROUPS: Dict[str, List[tuple[OS, Arch]]] = {
    # 32-bit x86 is deliberately absent: it is emulator-only, and build_ocean.py
    # keeps the same set so the two scripts agree on what "all Android" means.
    # `--target android_x86` still builds it for anyone who asks explicitly.
    "android": [
        (OS.ANDROID, Arch.ARM64),
        (OS.ANDROID, Arch.ARMV7),
        (OS.ANDROID, Arch.X86_64),
    ],
    "ios": [
        (OS.IOS, Arch.ARM64),
    ],
    "macos": [
        (OS.MACOS, Arch.ARM64),
        (OS.MACOS, Arch.X86_64),
    ],
    "linux": [
        (OS.LINUX, Arch.ARM64),
        (OS.LINUX, Arch.X86_64),
    ],
    "win": [
        (OS.WINDOWS, Arch.X86_64),
        (OS.WINDOWS, Arch.ARM64),
    ],
    "windows": [  # backward-compatible alias for "win"
        (OS.WINDOWS, Arch.X86_64),
        (OS.WINDOWS, Arch.ARM64),
    ],
}


def get_all_supported_platforms(
    vs_version: Optional[str] = None,
) -> tuple[List[tuple[OS, Arch]], List[tuple[str, str]]]:
    """Get all target platforms supported by the current host.

    Args:
        vs_version: Visual Studio year to scope Windows detection to. Ignored on
            non-Windows hosts.

    Returns a tuple of:
        - platforms: list of (OS, Arch) that can be built
        - skipped: list of (platform_group, reason) for platforms that were skipped

    On macOS:
        - macos_arm64 or macos_x86_64 (native)
        - ios_arm64 (if Xcode is available)
        - android_arm64, android_armv7, android_x86_64 (if NDK available)

    On Linux:
        - linux_arm64 or linux_x86_64 (native)
        - android_arm64, android_armv7, android_x86_64 (if NDK available)

    On Windows:
        - All architectures with installed MSVC tools (detected via vswhere)
        - Only 64-bit architectures (x86_64, arm64); x86 (32-bit) is excluded
        - android_arm64, android_armv7, android_x86_64 (if NDK available)
    """
    from lib.platform import (
        detect_host_arch,
        detect_host_os,
        get_android_ndk_path,
        get_ios_sdk_path,
    )

    platforms = []
    skipped = []
    host_os = detect_host_os()
    host_arch = detect_host_arch()

    # Include the native host target. On Windows this is decided by MSVC
    # toolchain detection below: an ARM64 host whose Visual Studio lacks the
    # ARM64 tools cannot build for its own architecture.
    if host_os != OS.WINDOWS:
        platforms.append((host_os, host_arch))

    if host_os == OS.MACOS:
        # macOS can cross-compile to iOS if Xcode is available
        if get_ios_sdk_path("iphoneos"):
            platforms.append((OS.IOS, Arch.ARM64))
        else:
            skipped.append(("ios", "Xcode not found (install Xcode from the App Store)"))

        # Also support x86_64 on arm64 Mac (and vice versa)
        if host_arch == Arch.ARM64:
            platforms.append((OS.MACOS, Arch.X86_64))
        elif host_arch == Arch.X86_64:
            platforms.append((OS.MACOS, Arch.ARM64))

    elif host_os == OS.WINDOWS:
        # Add only the 64-bit architectures whose MSVC tools are installed in
        # the Visual Studio installation this build will actually use.
        installed_archs, scoped = detect_windows_archs(vs_version)
        for arch in (Arch.X86_64, Arch.ARM64):
            if arch in installed_archs:
                platforms.append((OS.WINDOWS, arch))
            elif scoped:
                skipped.append(
                    (
                        f"win_{arch.value}",
                        f"{WINDOWS_ARCH_COMPONENTS[arch]} not installed in the "
                        "selected Visual Studio (add it via the Visual Studio "
                        "Installer, or pass --vs-version to select another "
                        "installation)",
                    )
                )
            else:
                # No installation could be identified, so the arch list above
                # is a host-architecture guess. Saying the component is "not
                # installed in the selected Visual Studio" would be a claim
                # the probe never made.
                skipped.append(
                    (
                        f"win_{arch.value}",
                        "could not identify a Visual Studio installation to "
                        "probe (vswhere.exe not found), so only the host "
                        f"architecture is assumed buildable — pass "
                        f"--target win_{arch.value} to try it anyway",
                    )
                )

    # All platforms can cross-compile to Android if NDK is available
    if get_android_ndk_path():
        platforms.extend(
            [
                (OS.ANDROID, Arch.ARM64),
                (OS.ANDROID, Arch.ARMV7),
                (OS.ANDROID, Arch.X86_64),
            ]
        )
    else:
        skipped.append(("android", "Android NDK not found (set ANDROID_NDK_HOME or ANDROID_NDK_ROOT)"))

    return platforms, skipped


def _dedup(values: List) -> List:
    """Order-preserving de-duplication."""
    return list(dict.fromkeys(values))


def _non_negative_int(value: str) -> int:
    """argparse type for job counts: reject negatives, keep 0 as "auto-detect"."""
    try:
        parsed = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"expected an integer, got {value!r}")
    if parsed < 0:
        raise argparse.ArgumentTypeError(
            f"must be 0 (auto-detect) or positive, got {parsed}"
        )
    return parsed


def _split_list_arg(values: Optional[List[str]]) -> List[str]:
    """Flatten repeated and comma-separated occurrences of a list-valued flag."""
    result = []
    for arg in values or []:
        for item in arg.split(","):
            item = item.strip()
            if item:
                result.append(item)
    return _dedup(result)


def write_expected_matrix(path: Path, libraries, targets) -> None:
    """Write the matrix of results `audit_link_variants.py` must find.

    The audit cannot infer what should exist from what does exist -- if nobody
    built Windows, a requirement derived from the results would quietly stop
    covering Windows. So the expectation is generated here, from the manifest,
    where the answer is actually known.

    Both link types are emitted for every library that supports both, whatever
    `--link` this particular invocation used: the matrix describes the intended
    build, not the one run that produced it. It is still scoped to `--target`,
    so generate it with the full set of targets the audit is meant to cover.
    """
    # A base target is one target minus its link type; the link types allowed
    # against it come from the manifest, not from this invocation's --link.
    base_os: Dict[str, str] = {}
    for target in targets:
        base_os[target_without_link_type(target.to_path_component())] = target.os.value

    entries = []
    for name, lib in sorted(libraries.items()):
        for base, os_value in sorted(base_os.items()):
            if not lib.supports_platform(os_value):
                continue
            kinds = [
                kind
                for kind in ("static", "shared")
                if lib.supports_link_type(kind)
                # iOS shared builds are excluded for code-signing and Swift
                # module reasons, so requiring one would never be satisfiable.
                and not (kind == "shared" and os_value == OS.IOS.value)
            ]
            if kinds:
                entries.append(
                    {
                        "library": name,
                        "version": lib.version,
                        "target": base,
                        "link_types": kinds,
                    }
                )

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps({"expected": entries}, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote expected matrix for {len(entries)} results to {path}")


def get_equivalent_command(
    args: argparse.Namespace,
    targets: List[BuildTarget],
    script_name: str = "build_ocean_3rdparty.py",
) -> str:
    """Spell out the fully explicit command that reproduces this run.

    Derived from the resolved target list rather than re-parsed from ``args``,
    so the printed command names the targets the build actually used — not the
    ones the user asked for before detection and the toolchain check pruned
    them. Pasting it back must produce the same build.
    """
    parts = [f"python {script_name}"]

    target_strs = _dedup([f"{t.os.value}_{t.arch.value}" for t in targets])
    parts.append(f"--target {','.join(target_strs)}")

    # Directories
    cwd = Path.cwd()
    base_dir = Path(args.output_dir) if args.output_dir else cwd / DEFAULT_BASE_DIR
    install_dir = Path(args.install_dir) if args.install_dir else base_dir / "install"
    source_dir = Path(args.source_dir) if args.source_dir else base_dir / "source"
    build_dir = Path(args.build_dir) if args.build_dir else base_dir / "build"
    parts.append(f"--install-dir {install_dir}")
    parts.append(f"--source-dir {source_dir}")
    parts.append(f"--build-dir {build_dir}")

    config_strs = _dedup([t.build_config.value for t in targets])
    parts.append(f"--config {','.join(config_strs)}")

    link_strs = _dedup([t.link_type.value for t in targets])
    parts.append(f"--link {','.join(link_strs)}")

    if args.manifest:
        parts.append(f"--manifest {args.manifest}")

    # Flags that change which libraries are built or where they end up. Omitting
    # any of these makes the printed command build a different set than the run
    # it claims to reproduce.
    libs = _split_list_arg(getattr(args, "library", None))
    if libs:
        parts.append(f"--library {','.join(libs)}")
    for name in _split_list_arg(args.with_libs):
        parts.append(f"--with {name}")
    for name in _split_list_arg(args.with_group):
        parts.append(f"--with-group {name}")
    if args.build_all:
        parts.append("--all")
    if args.for_external_integration:
        parts.append("--for-external-integration")
    if args.with_cmake_configs:
        parts.append("--with-cmake-configs")
    if args.vs_version:
        parts.append(f"--vs-version {args.vs_version}")
    if args.android_api_level:
        parts.append(f"--android-api-level {args.android_api_level}")
    if args.parallel:
        parts.append(f"--parallel {args.parallel}")
    if args.jobs:
        parts.append(f"--jobs {args.jobs}")

    return " \\\n    ".join(parts)


# ============================================================================
# Build Job and Statistics
# ============================================================================


@dataclass
class BuildJob:
    """A single build job: one library × one target."""

    library: LibraryConfig
    target: BuildTarget


@dataclass
class BuildResult:
    """Result of a single build job, including timing."""

    library_name: str
    target: str
    install_path: Path
    duration_seconds: float


@dataclass
class BuildStats:
    """Statistics for the entire build run."""

    results: List[BuildResult] = field(default_factory=list)
    start_time: float = field(default_factory=time.time)
    end_time: float = 0.0

    def add_result(self, result: BuildResult) -> None:
        """Add a build result."""
        self.results.append(result)

    def finish(self) -> None:
        """Mark build as finished."""
        self.end_time = time.time()

    @property
    def total_duration(self) -> float:
        """Total wall-clock duration in seconds."""
        return self.end_time - self.start_time

    def print_summary(self) -> None:
        """Print build statistics summary."""
        if not self.results:
            return

        print(f"\n{'═' * 70}")
        print("Build Statistics")
        print(f"{'═' * 70}")

        # Group by library
        lib_times: Dict[str, List[float]] = {}
        for r in self.results:
            lib_times.setdefault(r.library_name, []).append(r.duration_seconds)

        # Sort by total time (descending)
        lib_totals = [(lib, sum(times)) for lib, times in lib_times.items()]
        lib_totals.sort(key=lambda x: -x[1])

        print(f"\n{'Library':<25} {'Builds':>8} {'Total':>12} {'Avg':>12}")
        print(f"{'-' * 25} {'-' * 8} {'-' * 12} {'-' * 12}")

        for lib, total in lib_totals:
            count = len(lib_times[lib])
            avg = total / count
            print(
                f"{lib:<25} {count:>8} {_format_duration(total):>12} {_format_duration(avg):>12}"
            )

        # Overall summary
        total_build_time = sum(r.duration_seconds for r in self.results)
        print(f"\n{'─' * 70}")
        print(f"Total builds:         {len(self.results)}")
        print(f"Cumulative build time: {_format_duration(total_build_time)}")
        print(f"Wall-clock time:       {_format_duration(self.total_duration)}")
        print(f"Parallelism factor:    {total_build_time / self.total_duration:.1f}x")


def _format_duration(seconds: float) -> str:
    """Format duration in human-readable form."""
    if seconds < 60:
        return f"{seconds:.1f}s"
    elif seconds < 3600:
        minutes = int(seconds // 60)
        secs = seconds % 60
        return f"{minutes}m {secs:.0f}s"
    else:
        hours = int(seconds // 3600)
        minutes = int((seconds % 3600) // 60)
        return f"{hours}h {minutes}m"


def get_builder(system: str) -> Builder:
    """Get the appropriate builder for a build system."""
    if system == "cmake":
        return CMakeBuilder()
    elif system == "header_only":
        return HeaderOnlyBuilder()
    elif system == "imported_shared":
        return ImportedSharedBuilder()
    else:
        raise ValueError(f"Unknown build system: {system}")


def execute_build_job(
    job: BuildJob,
    dir_manager: DirectoryManager,
    fetcher: SourceFetcher,
    completed_libs: Dict[str, Path],
    version_map: Dict[str, str],
    jobs_per_lib: int,
    include_cmake_configs: bool = False,
    log_level: LogLevel = LogLevel.STATUS,
    progress: Optional[ProgressDisplay] = None,
    vs_version: Optional[str] = None,
    android_api_level: Optional[int] = None,
) -> BuildResult:
    """Execute a single build job.

    Args:
        job: The build job to execute
        dir_manager: Directory manager
        fetcher: Source fetcher
        completed_libs: Map of completed library names to their install paths
        version_map: Map of library names to versions
        jobs_per_lib: Number of parallel compile jobs per library
        include_cmake_configs: If True, include CMake/pkg-config files
        log_level: Log level for build output
        progress: Progress display for TUI updates
        vs_version: Visual Studio version to use (e.g., "2022", "2026")
        android_api_level: Android API level (e.g., 24, 32, 34)

    Returns:
        BuildResult with path and timing information
    """
    start_time = time.time()
    lib = job.library
    target = job.target

    # Extract target components for progress display
    # Include both OS and arch to ensure unique keys (e.g., "ios_arm64", "android_x86")
    platform = f"{target.os.value}_{target.arch.value}"
    config = target.build_config.value
    link = target.link_type.value

    def report_phase(phase: BuildPhase) -> None:
        if progress:
            progress.update_phase(lib.name, platform, config, link, phase)

    # Start progress tracking
    if progress:
        progress.start_job(lib.name, platform, config, link, BuildPhase.FETCHING)

    # Fetch source (cached, thread-safe)
    # Use quiet mode when progress display is active to avoid interfering with TUI
    fetcher.fetch(lib.name, lib.version, lib.source, quiet=progress is not None)

    # Get build paths
    paths = dir_manager.get_paths(lib.name, lib.version, target)

    # Get dependency directories
    dep_dirs = dir_manager.get_dependency_dirs(lib.dependencies, version_map, target)

    # Get merged build options (common + link-type + config specific)
    merged_options = lib.build.get_merged_options(
        link_type=target.link_type.value,
        config=target.build_config.value,
    )

    # Determine source directory (may be a subdirectory of fetched source)
    actual_source_dir = paths.source_dir
    if lib.source.source_subdir:
        actual_source_dir = paths.source_dir / lib.source.source_subdir

    # Create build context with progress callback
    def progress_callback(phase_name: str) -> None:
        phase_map = {
            "configuring": BuildPhase.CONFIGURING,
            "building": BuildPhase.BUILDING,
            "installing": BuildPhase.INSTALLING,
        }
        phase = phase_map.get(phase_name.lower(), BuildPhase.BUILDING)
        report_phase(phase)

    # Set up log file in build directory
    log_file = paths.build_dir / "build.log"

    ctx = BuildContext(
        library_name=lib.name,
        source_dir=actual_source_dir,
        build_dir=paths.build_dir,
        install_dir=paths.install_dir,
        target=target,
        dependency_dirs=dep_dirs,
        build_options=merged_options,
        jobs=jobs_per_lib,
        log_level=log_level,
        progress_callback=progress_callback if progress else None,
        log_file=log_file,
        vs_version=vs_version,
        android_api_level=android_api_level,
        post_install_copy=lib.build.post_install_copy or None,
    )

    # Get builder
    builder = get_builder(lib.build.system)

    # Build with progress reporting
    report_phase(BuildPhase.CONFIGURING)
    builder.configure(ctx)

    report_phase(BuildPhase.BUILDING)
    builder.build(ctx)

    report_phase(BuildPhase.INSTALLING)
    # cmake --install and equivalent installers are additive. Recreate this
    # job's private staging prefix so removed artifacts cannot be republished.
    if paths.install_dir.exists():
        remove_tree(paths.install_dir)
    builder.install(ctx)

    # Post-install: either reorganize for external integration, or preserve
    # the standard CMake install tree under <install_root>/<target>/<library>/.
    report_phase(BuildPhase.REORGANIZING)
    if dir_manager.for_external_integration:
        reorganize_output(
            paths.install_dir,
            paths.final_dir,
            target,
            lib.name,
            dir_manager.install_dir,
            include_cmake_configs,
        )
    else:
        _install_standard_layout(paths.install_dir, paths.final_dir, target)

    # Write metadata
    source_info = {
        "type": lib.source.type,
        "url": lib.source.url,
        "ref": lib.source.ref,
        "fetched_commit": fetcher.get_actual_commit(lib.name, lib.version),
    }
    dir_manager.write_build_metadata(
        lib.name, lib.version, target, source_info, android_api_level
    )

    duration = time.time() - start_time
    return BuildResult(
        library_name=lib.name,
        target=target.to_path_component(),
        install_path=paths.final_dir,
        duration_seconds=duration,
    )


# Lock for include directory operations (shared across targets with same platform)
import threading

_include_locks: dict = {}
_include_locks_lock = threading.Lock()


def _get_include_lock(include_dir: Path) -> threading.Lock:
    """Get or create a lock for a platform's include directory."""
    key = str(include_dir)
    with _include_locks_lock:
        if key not in _include_locks:
            _include_locks[key] = threading.Lock()
        return _include_locks[key]


def _get_base_lib_name(filename: str) -> str:
    """Get the base library name without version numbers.

    Examples:
        libz.1.3.1.dylib -> libz.dylib
        libz.1.dylib -> libz.dylib
        libcurl.4.dylib -> libcurl.dylib
        libpng16.16.dylib -> libpng16.dylib
        libz.so.1.3.1 -> libz.so
        libz.so.1 -> libz.so
    """
    import re

    # Handle .dylib with version numbers: libfoo.1.2.3.dylib -> libfoo.dylib
    dylib_match = re.match(r"^(lib[^.]+)(\.\d+)+\.dylib$", filename)
    if dylib_match:
        return f"{dylib_match.group(1)}.dylib"

    # Handle .so with version numbers: libfoo.so.1.2.3 -> libfoo.so
    so_match = re.match(r"^(lib[^.]+\.so)(\.\d+)+$", filename)
    if so_match:
        return so_match.group(1)

    return filename


def _is_versioned_lib(filename: str) -> bool:
    """Check if a library filename contains version numbers.

    Returns True for versioned library names like:
        libz.1.3.1.dylib, libz.1.dylib, libz.so.1.3.1, libz.so.1
        libwx_baseu-3.3.1d.dylib, libwx_baseu-3.3.1.0.0d.dylib
    Returns False for unversioned names like:
        libz.dylib, libz.so, libz.a
        libwx_baseu-3.3d.dylib (2-component version is the base)
        libwx_baseu-3.3.dylib (2-component version is the base)
    """
    import re

    # Skip wxWidgets-style names with hyphen-version pattern for pattern 1
    # These use -X.X format which should not be treated as .X.X versioning
    if "-" in filename and re.search(r"-\d+\.\d+", filename):
        # This is a wxWidgets-style library, only check for 3+ version components
        # libwx_baseu-3.3.dylib -> keep (2 components)
        # libwx_baseu-3.3.1.dylib -> skip (3 components)
        if re.match(r"^lib.*-\d+\.\d+\.\d+.*\.dylib$", filename):
            return True
        return False

    # Match .dylib with version numbers after library name: libfoo.1.2.3.dylib
    if re.match(r"^lib[^.]+(\.\d+)+\.dylib$", filename):
        return True

    # Match .so with version numbers: libfoo.so.1.2.3
    if re.match(r"^lib[^.]+\.so(\.\d+)+$", filename):
        return True

    return False


def _fix_macos_install_names(lib_dir: Path) -> None:
    """Fix install names in macOS shared libraries to use unversioned names.

    For each .dylib in lib_dir:
    1. Change the library's own install name (LC_ID_DYLIB) to unversioned
    2. Change references to other versioned libraries to unversioned names

    This allows binaries to find libraries by their unversioned names at runtime.
    """
    import subprocess

    dylibs = [f for f in lib_dir.iterdir() if f.suffix == ".dylib" and f.is_file()]

    for dylib in dylibs:
        try:
            # Get current install name and dependencies using otool
            result = subprocess.run(
                ["otool", "-L", str(dylib)],
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                check=True,
            )

            changes = []
            for line in result.stdout.split("\n")[1:]:  # Skip first line (filename)
                line = line.strip()
                if not line:
                    continue
                # Format: "/path/to/lib.dylib (compatibility version ...)"
                lib_path = line.split(" (")[0].strip()
                lib_name = Path(lib_path).name

                if _is_versioned_lib(lib_name):
                    base_name = _get_base_lib_name(lib_name)
                    # Construct new path with unversioned name
                    new_path = lib_path.replace(lib_name, base_name)
                    changes.append((lib_path, new_path))

            # Apply changes using install_name_tool
            if changes:
                cmd = ["install_name_tool"]
                for old_path, new_path in changes:
                    cmd.extend(["-change", old_path, new_path])
                cmd.append(str(dylib))
                subprocess.run(cmd, check=True, capture_output=True)

            # Also fix the library's own install name (LC_ID_DYLIB)
            # Get the current ID
            result = subprocess.run(
                ["otool", "-D", str(dylib)],
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                check=True,
            )
            lines = result.stdout.strip().split("\n")
            if len(lines) >= 2:
                current_id = lines[1].strip()
                current_name = Path(current_id).name
                if _is_versioned_lib(current_name):
                    base_name = _get_base_lib_name(current_name)
                    new_id = current_id.replace(current_name, base_name)
                    subprocess.run(
                        ["install_name_tool", "-id", new_id, str(dylib)],
                        check=True,
                        capture_output=True,
                    )
        except subprocess.CalledProcessError:
            # If install_name_tool fails, continue with other libraries
            pass


# Extensions allowed in the final install directory
ALLOWED_HEADER_EXTENSIONS = {
    ".h",
    ".hpp",
    ".hxx",
    ".inl",
    ".inc",
    ".cpp",
    ".c",
    ".cxx",
    ".cc",
}
UNWANTED_EXTENSIONS = {".md", ".json", ".lock", ".cmake", ".txt", ".rst", ".3", ".pc"}


def _copy_headers_filtered(src_dir: Path, dest_dir: Path) -> None:
    """Copy header files, filtering out unwanted file types.

    Copies files with allowed header extensions and extension-less files
    (e.g., Eigen's Dense, Sparse, Core headers).
    Skips documentation, cmake files, man pages, etc.
    """
    import shutil

    def ignore_unwanted(directory: str, files: list) -> list:
        """Return list of files to ignore."""
        ignored = []
        for f in files:
            path = Path(directory) / f
            # Don't ignore directories (we want to traverse them)
            if path.is_dir():
                continue
            ext = path.suffix.lower()
            # Keep files with allowed header extensions
            if ext in ALLOWED_HEADER_EXTENSIONS:
                continue
            # Keep extension-less files (e.g., Eigen's Dense, Sparse, Core)
            if ext == "":
                continue
            # Skip known unwanted extensions
            ignored.append(f)
        return ignored

    shutil.copytree(src_dir, dest_dir, ignore=ignore_unwanted)


def _copy_lib_files_recursive(
    src_dir: Path,
    dest_dir: Path,
    skip_extensions: set,
    skip_patterns: list,
    skip_versioned: bool = False,
) -> None:
    """Recursively copy library files from src_dir to dest_dir (flattened).

    Skips .framework directories and files matching skip_extensions/skip_patterns.
    All library files are copied directly to dest_dir regardless of subdirectory depth.
    For symlinks, copies the target file using the symlink's name (for CMake compatibility).
    If skip_versioned is True, skips versioned library files (e.g., libz.1.dylib).
    """
    import shutil

    # Collect symlink targets to skip real files that have an unversioned alias
    symlink_targets = set()
    for item in src_dir.iterdir():
        if item.is_symlink():
            symlink_targets.add(item.resolve())

    for item in src_dir.iterdir():
        if item.is_dir() and not item.is_symlink():
            # Skip .framework directories
            if item.suffix == ".framework":
                continue
            # Recurse into other directories
            _copy_lib_files_recursive(
                item, dest_dir, skip_extensions, skip_patterns, skip_versioned
            )
        elif item.is_file() or item.is_symlink():
            # For symlinks, we'll copy the target file using the symlink's name
            # This preserves CMake-expected names like libpng.dylib
            actual_file = item.resolve() if item.is_symlink() else item

            # Skip if the symlink target doesn't exist
            if not actual_file.exists():
                continue

            # Skip real files that have a symlink alias (prefer the
            # unversioned symlink name, e.g., libpng.a over libpng16.a)
            if not item.is_symlink() and actual_file in symlink_targets:
                continue

            # Filter out unwanted library files
            skip_file = False
            # Skip based on link type (static vs shared)
            if item.suffix in skip_extensions:
                skip_file = True
            # Skip unwanted file types (docs, cmake, etc.)
            if item.suffix.lower() in UNWANTED_EXTENSIONS:
                skip_file = True
            # Skip versioned library files (e.g., libz.1.dylib, libz.so.1)
            if skip_versioned and _is_versioned_lib(item.name):
                skip_file = True
            for pattern in skip_patterns:
                if pattern in item.name:
                    skip_file = True
                    break
            if not skip_file:
                dest_name = item.name
                dest = dest_dir / dest_name
                # Only copy if we don't already have this library
                if not dest.exists():
                    shutil.copy2(actual_file, dest)


def _detect_installed_library_kinds(install_dir: Path) -> tuple[bool, bool]:
    """Report whether an install tree contains static and/or shared libraries.

    The wrong-link-type filter must only run when a tree contains both kinds —
    that is, when an upstream project installed a variant we did not ask for
    (libpng installs both regardless of BUILD_SHARED_LIBS). Applying it
    unconditionally empties libraries that only ever ship one kind, such as an
    imported_shared library like ARCore, which is nothing but a .so.
    """
    has_static = any(install_dir.rglob("*.a")) or any(install_dir.rglob("*.lib"))
    has_shared = (
        any(install_dir.rglob("*.so"))
        or any(install_dir.rglob("*.dylib"))
        or any(install_dir.rglob("*.dll"))
    )
    return has_static, has_shared


def _install_standard_layout(
    install_dir: Path,
    final_dir: Path,
    target: BuildTarget,
) -> None:
    """Copy the CMake-installed tree to its final per-target prefix.

    Standard layout preserves the install tree as cmake --install produced
    it (include/, lib/, share/, bin/, …) so the destination is a complete,
    relocatable CMake install prefix that find_package(CONFIG) can consume
    directly via CMAKE_PREFIX_PATH.

    Wrong-link-type artifacts are filtered out (e.g., .dylib/.so/.dll when
    the target requested static linking, .a when shared). Some upstream
    CMake projects install both regardless of BUILD_SHARED_LIBS; the copy
    keeps only the requested kind so consumers cannot accidentally link
    the wrong variant.

    Windows is exempt. There the filter cannot work: a static library and the
    import library for a DLL are both `.lib`, so an extension test cannot tell
    them apart. Filtering a static target dropped `foo.dll` and kept both
    `foo.lib` (its import library) and `foostatic.lib`, leaving a prefix that
    links successfully and then fails at process start with a missing DLL --
    a state that would not exist if nothing had been filtered at all. Keeping
    both variants is untidy; publishing a dangling import library is broken.

    Args:
        install_dir: Source directory containing cmake --install output.
        final_dir: Destination directory (<install_root>/<target>/<library>/).
        target: Build target (used to decide which link-type artifacts to keep).
    """
    import shutil

    if not install_dir.exists():
        return

    has_static_libs, has_shared_libs = _detect_installed_library_kinds(install_dir)

    is_shared = target.link_type == LinkType.SHARED
    skip_patterns: List[str] = []
    if target.os == OS.WINDOWS:
        skip_extensions = set()
    elif is_shared and has_static_libs and has_shared_libs:
        skip_extensions = {".a"}
    elif not is_shared and has_static_libs and has_shared_libs:
        skip_extensions = {".dylib", ".so", ".dll"}
        skip_patterns = [".so."]
    else:
        skip_extensions = set()

    def should_skip(path: Path) -> bool:
        # Only the wrong-link-type artifacts are dropped. Versioned names are
        # deliberately kept: for a shared build `libz.so.1.3.1` /
        # `libz.1.3.1.dylib` IS the library — the unversioned name is only a
        # symlink to it — so filtering versioned files here would copy the
        # symlink chain and none of its targets.
        if path.suffix in skip_extensions:
            return True
        for pattern in skip_patterns:
            if pattern in path.name:
                return True
        return False

    def ignore(directory: str, names: List[str]) -> List[str]:
        ignored: List[str] = []
        for name in names:
            path = Path(directory) / name
            # Skip .framework bundles entirely. On macOS some libraries
            # (e.g., libpng) install a Foo.framework alongside the static
            # archive, and CMake's Find modules prefer the framework. For
            # static builds we want only the .a, and for shared builds
            # the bare .dylib in lib/ already covers us.
            if path.is_dir() and name.endswith(".framework"):
                ignored.append(name)
                continue
            if path.is_dir() or path.is_symlink():
                continue
            if should_skip(path):
                ignored.append(name)
        return ignored

    if final_dir.exists():
        remove_tree(final_dir)
    final_dir.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(install_dir, final_dir, symlinks=True, ignore=ignore)

    # After copying with symlinks, drop wrong-link-type symlinks that point
    # at files we just filtered out (e.g., libz.dylib -> libz.1.dylib).
    for path in final_dir.rglob("*"):
        if path.is_symlink() and should_skip(path):
            path.unlink()


def reorganize_output(  # noqa: C901
    install_dir: Path,
    final_dir: Path,
    target: BuildTarget,
    library_name: str,
    top_level_install_dir: Path,
    include_cmake_configs: bool = False,
) -> None:
    """Reorganize CMake install output to final structure.

    CMake installs to:
        install_dir/
            include/
            lib/

    We want:
        final_dir/
            h/
                ios/              # OS-only (headers same across architectures)
            lib/
                ios_arm64_static/ # Target-specific

    CMake/pkg-config files go to central location:
        top_level_install_dir/
            cmake/{library}/
            pkgconfig/{library}/

    Args:
        install_dir: CMake install directory
        final_dir: Final output directory for library
        target: Build target
        library_name: Name of the library being built
        top_level_install_dir: Top-level install directory for central cmake/pkgconfig
        include_cmake_configs: If True, copy CMake/pkg-config files
    """
    import shutil

    target_str = target.to_path_component()
    platform_str = target.to_platform_component()

    # Create final directories
    final_dir.mkdir(parents=True, exist_ok=True)
    final_include = final_dir / "h" / platform_str
    final_lib = final_dir / "lib" / target_str

    # Copy include to platform-specific directory (OS only, not arch/config/link)
    # Headers don't differ between architectures, debug/release, or static/shared
    # Use locking to prevent race when multiple targets copy same platform includes
    src_include = install_dir / "include"
    if src_include.exists():
        lock = _get_include_lock(final_include)
        with lock:
            if not final_include.exists():
                final_include.parent.mkdir(parents=True, exist_ok=True)
                _copy_headers_filtered(src_include, final_include)

    # Copy lib to target-specific directory (exclude cmake/pkgconfig)
    # Also filter out wrong library types (e.g., .dylib when building static)
    #
    # Both lib/ and lib64/ are read. GNUInstallDirs resolves CMAKE_INSTALL_LIBDIR
    # to lib64 for 64-bit builds on RHEL-family distributions, so a tree that
    # installed there looks empty to a lib-only search and the library silently
    # reduces to headers. Everything is flattened into a single lib/<target>
    # directory, which is what the external layout has always published.
    src_libs = [d for d in (install_dir / "lib", install_dir / "lib64") if d.is_dir()]
    is_shared = target.link_type == LinkType.SHARED

    if src_libs:
        if final_lib.exists():
            remove_tree(final_lib)
        final_lib.mkdir(parents=True, exist_ok=True)

        entries = [item for src_lib in src_libs for item in src_lib.iterdir()]

        # Collect symlink targets so we skip real files that have an unversioned
        # symlink alias (e.g., skip libpng16.a when libpng.a -> libpng16.a exists)
        symlink_targets = {item.resolve() for item in entries if item.is_symlink()}

        # Determine which library extensions to skip based on link type. Only
        # filter when the install tree actually holds both kinds; a library
        # that ships a single kind (an imported_shared .so like ARCore) would
        # otherwise be filtered down to an empty directory.
        has_static_libs, has_shared_libs = _detect_installed_library_kinds(install_dir)
        if target.os == OS.WINDOWS:
            # A static library and a DLL's import library are both `.lib`, so
            # the extension cannot distinguish them and filtering a static
            # target deleted the DLL while keeping its import library. See
            # _install_standard_layout for the full reasoning.
            skip_extensions = set()
            skip_patterns = []
        elif not (has_static_libs and has_shared_libs):
            skip_extensions = set()
            skip_patterns = []
        elif target.link_type == LinkType.STATIC:
            # Skip shared libraries when building static
            # On Windows, DLLs are in bin/ so we don't see them here, but skip .dll just in case
            skip_extensions = {".dylib", ".so", ".dll"}
            # Also skip versioned .so files like libz.so.1, libz.so.1.3.1
            skip_patterns = [".so."]
        else:
            # Skip static libraries when building shared
            # On Windows, .lib files are import libraries (needed), not static libraries
            # On Unix, .a files are static libraries that should be skipped
            skip_extensions = {".a"}
            skip_patterns = []

        for item in entries:
            # Always skip cmake and pkgconfig in lib directory
            if item.is_dir() and item.name in ("cmake", "pkgconfig"):
                if include_cmake_configs:
                    # Copy to central location instead
                    central_dir = top_level_install_dir / item.name / library_name
                    if central_dir.exists():
                        remove_tree(central_dir)
                    shutil.copytree(item, central_dir)
            elif item.is_dir():
                # Skip .framework directories entirely
                if item.suffix == ".framework":
                    continue
                # Flatten other subdirectories - copy their library files directly
                # For shared builds, skip versioned files (e.g., libz.1.dylib)
                _copy_lib_files_recursive(
                    item,
                    final_lib,
                    skip_extensions,
                    skip_patterns,
                    skip_versioned=is_shared,
                )
            elif item.is_file() or item.is_symlink():
                # For symlinks, we'll copy the target file using the symlink's name
                # This preserves CMake-expected names like libpng.dylib
                actual_file = item.resolve() if item.is_symlink() else item

                # Skip if the symlink target doesn't exist
                if not actual_file.exists():
                    continue

                # Skip real files that have a symlink alias (prefer the
                # unversioned symlink name, e.g., libpng.a over libpng16.a)
                if not item.is_symlink() and actual_file in symlink_targets:
                    continue

                # Filter out unwanted library files
                skip_file = False
                # Skip based on link type (static vs shared)
                if item.suffix in skip_extensions:
                    skip_file = True
                # Skip unwanted file types (docs, cmake, etc.)
                if item.suffix.lower() in UNWANTED_EXTENSIONS:
                    skip_file = True
                # For shared builds, skip versioned library files
                if is_shared and _is_versioned_lib(item.name):
                    skip_file = True
                for pattern in skip_patterns:
                    if pattern in item.name:
                        skip_file = True
                        break
                if not skip_file:
                    dest_name = item.name
                    dest = final_lib / dest_name
                    # Only copy if we don't already have this library
                    if not dest.exists():
                        shutil.copy2(actual_file, dest)

    # For shared builds on macOS/iOS, fix install names to use unversioned library names
    if target.link_type == LinkType.SHARED and target.os in (OS.MACOS, OS.IOS):
        if final_lib.exists():
            _fix_macos_install_names(final_lib)

    # On Windows, CMake places DLLs in bin/ directory, not lib/
    # Copy DLLs to the final lib directory for shared builds
    # Note: Some projects (like wxWidgets) put DLLs in subdirectories of bin/
    if target.link_type == LinkType.SHARED and target.os == OS.WINDOWS:
        src_bin = install_dir / "bin"
        if src_bin.exists():
            final_lib.mkdir(parents=True, exist_ok=True)
            # Recursively find all DLLs in bin/ and its subdirectories
            for dll_file in src_bin.rglob("*.dll"):
                if dll_file.is_file():
                    dest_name = dll_file.name
                    dest = final_lib / dest_name
                    if not dest.exists():
                        shutil.copy2(dll_file, dest)

    # Special handling for wxWidgets: setup.h needs to be in a wx/ subdirectory
    # so that wxWidgets can include it as <wx/setup.h>
    if library_name == "wxwidgets":
        setup_h = final_lib / "setup.h"
        if setup_h.exists():
            wx_subdir = final_lib / "wx"
            wx_subdir.mkdir(exist_ok=True)
            shutil.move(str(setup_h), str(wx_subdir / "setup.h"))

    # Also copy CMake config files from other locations to central location
    if include_cmake_configs:
        for cmake_dir in ["share/cmake", "cmake"]:
            src_cmake = install_dir / cmake_dir
            if src_cmake.exists():
                central_cmake = top_level_install_dir / "cmake" / library_name
                if not central_cmake.exists():
                    shutil.copytree(src_cmake, central_cmake)
                break

        # Copy pkg-config files from share/pkgconfig to central location
        src_pkgconfig = install_dir / "share" / "pkgconfig"
        if src_pkgconfig.exists():
            central_pkgconfig = top_level_install_dir / "pkgconfig" / library_name
            if not central_pkgconfig.exists():
                shutil.copytree(src_pkgconfig, central_pkgconfig)


# ============================================================================
# Main Build Orchestrator
# ============================================================================


def build_all(
    manifest: Manifest,
    libraries: Dict[str, LibraryConfig],
    targets: List[BuildTarget],
    dir_manager: DirectoryManager,
    fetcher: SourceFetcher,
    max_parallel: int = 0,
    jobs_per_lib: int = 0,
    include_cmake_configs: bool = False,
    log_level: LogLevel = LogLevel.STATUS,
    vs_version: Optional[str] = None,
    android_api_level: Optional[int] = None,
) -> BuildStats:
    """Build all libraries for all targets with maximum parallelism.

    Parallelism is across BOTH:
    - Libraries at the same DAG level (no dependencies between them)
    - Targets for each library (completely independent builds)

    Args:
        manifest: The manifest
        libraries: Libraries to build (filtered)
        targets: Build targets
        dir_manager: Directory manager
        fetcher: Source fetcher
        max_parallel: Maximum parallel jobs (0 = auto)
        jobs_per_lib: Compile jobs per library (0 = auto)
        include_cmake_configs: If True, include CMake/pkg-config files
        log_level: Log level for build output
        vs_version: Visual Studio version to use (e.g., "2022", "2026")
        android_api_level: Android API level (e.g., 24, 32, 34)

    Returns:
        BuildStats with timing information for all builds
    """
    stats = BuildStats()

    if max_parallel <= 0:
        max_parallel = os.cpu_count() or 8

    if jobs_per_lib <= 0:
        # Use fewer jobs per lib if we're building many in parallel
        jobs_per_lib = max(1, (os.cpu_count() or 8) // 4)

    # Build dependency graph
    graph = DependencyGraph.from_manifest(manifest, libraries)
    levels = graph.get_build_levels()

    # Create version map
    version_map = {name: lib.version for name, lib in libraries.items()}

    # Track completed libraries per target
    completed: Dict[str, Dict[str, Path]] = {
        target.to_path_component(): {} for target in targets
    }

    # Calculate total jobs, accounting for platform-specific libraries
    total_jobs = sum(
        1
        for level in levels
        for lib_name in level
        for target in targets
        if libraries[lib_name].supports_platform(target.os.value)
        and libraries[lib_name].supports_link_type(target.link_type.value)
    )
    completed_jobs = 0

    print(
        f"\nBuilding {len(libraries)} libraries × {len(targets)} targets = {total_jobs} jobs"
    )
    print(f"Max parallel: {max_parallel}, jobs per lib: {jobs_per_lib}\n")

    # Create progress display
    progress = ProgressDisplay(total_jobs)

    first_error: Optional[BaseException] = None

    try:
        for level_idx, level_libs in enumerate(levels):
            # Create all jobs for this level, filtering by platform and link type support
            jobs: List[BuildJob] = [
                BuildJob(library=libraries[lib_name], target=target)
                for lib_name in level_libs
                for target in targets
                if libraries[lib_name].supports_platform(target.os.value)
                and libraries[lib_name].supports_link_type(target.link_type.value)
            ]

            if not jobs:
                continue

            level_lib_count = len({j.library.name for j in jobs})
            level_target_count = len({j.target.to_path_component() for j in jobs})
            level_libraries = sorted({j.library.name for j in jobs})

            # Print level header through progress display
            progress.print_level_header(
                level_idx,
                level_lib_count,
                level_target_count,
                len(jobs),
                level_libraries,
            )

            # Execute all jobs in parallel. Not a `with` block: its __exit__
            # calls shutdown(wait=True) without cancel_futures, so every queued
            # job would still run to completion after a failure.
            executor = ThreadPoolExecutor(max_workers=min(max_parallel, len(jobs)))
            futures: Dict = {}
            try:
                futures = {
                    executor.submit(
                        execute_build_job,
                        job,
                        dir_manager,
                        fetcher,
                        completed[job.target.to_path_component()],
                        version_map,
                        jobs_per_lib,
                        include_cmake_configs,
                        log_level,
                        progress,
                        vs_version,
                        android_api_level,
                    ): job
                    for job in jobs
                }

                for future in as_completed(futures):
                    job = futures[future]
                    target = job.target
                    target_str = target.to_path_component()
                    # Include both OS and arch to match the key used in start_job
                    platform = f"{target.os.value}_{target.arch.value}"
                    config = target.build_config.value
                    link = target.link_type.value
                    try:
                        result = future.result()
                        completed[target_str][job.library.name] = result.install_path
                        stats.add_result(result)
                        completed_jobs += 1
                        progress.complete_job(
                            job.library.name,
                            platform,
                            config,
                            link,
                            result.duration_seconds,
                        )
                    except Exception as e:
                        progress.fail_job(
                            job.library.name,
                            platform,
                            config,
                            link,
                            str(e),
                        )
                        # Stop scheduling instead of re-raising here: raising
                        # from inside the loop leaves the remaining queued jobs
                        # to run to completion, so the build appears to carry on
                        # for another twenty minutes after the fatal error and
                        # only the first failure is ever reported.
                        if first_error is None:
                            first_error = e
                        for pending in futures:
                            pending.cancel()
                        break
            except KeyboardInterrupt as e:
                for pending in futures:
                    pending.cancel()
                first_error = e
                raise
            finally:
                # wait=True so in-flight compilers finish rather than being
                # orphaned; cancel_futures drops everything not yet started.
                executor.shutdown(wait=True, cancel_futures=True)

            if first_error is not None:
                break
    finally:
        # Always tear the live region down, even on failure or Ctrl-C: Rich
        # hides the cursor while it is running, and skipping stop() leaves the
        # user's shell with an invisible cursor and a stale display.
        progress.stop()

    if first_error is not None:
        _cleanup_lock_files(dir_manager.install_dir)
        raise first_error

    stats.finish()
    progress.print_summary()
    print(f"\n{'═' * 70}")
    print(f"Completed: {completed_jobs} builds")
    print(f"{'═' * 70}")

    # Print statistics
    stats.print_summary()

    # Clean up lock files from install directory
    _cleanup_lock_files(dir_manager.install_dir)

    return stats


def _cleanup_lock_files(install_dir: Path) -> None:
    """Remove .build_metadata.lock files from the install directory.

    These lock files are only needed during parallel builds to prevent
    race conditions. They have no purpose after the build completes.

    Searches recursively because the metadata files can live at depth 1
    (external-integration layout: <install_root>/<library>/) or depth 2
    (standard layout: <install_root>/<target>/<library>/).
    """
    for lock_file in install_dir.rglob(".build_metadata.lock"):
        try:
            lock_file.unlink()
        except OSError:
            pass  # Ignore errors (file may already be deleted)


# ============================================================================
# CLI
# ============================================================================


def _print_link_type_skip_warning(
    lib_name: str,
    skipped_targets: List[BuildTarget],
) -> None:
    """Print a prominent warning when a library will be skipped for some targets
    due to a link_types restriction in the manifest."""
    WIDTH = 70
    SEP = "═" * WIDTH

    explanations: Dict[str, List[str]] = {
        "mbedtls": [
            "mbedtls cannot be built as a shared library for Windows targets.",
            "It lacks __declspec(dllexport) declarations, so its output DLLs",
            "(mbedcrypto, mbedx509, mbedtls) would have no exported symbols and",
            "their inter-dependencies would fail to link. This is a Windows",
            "target ABI restriction and does not apply to Android or other",
            "non-Windows targets.",
            "",
            "To also get mbedtls for Windows, add '--link static' to your command.",
        ],
        "android_native_app_glue": [
            "android_native_app_glue must always be a static library.",
            "It provides the android_main() entry point and the NativeActivity",
            "event loop, which the OS resolves at load time inside the app's",
            "main .so. It is designed to be compiled directly into that .so —",
            "not distributed as a separate shared library. The NDK ships it as",
            "source only for this reason.",
        ],
    }

    lines = explanations.get(
        lib_name,
        [f"'{lib_name}' does not support the requested link type for these targets."],
    )
    target_strs = ", ".join(t.to_path_component() for t in skipped_targets)

    print(f"\n{SEP}")
    print(f"  WARNING: {lib_name} — skipped for {len(skipped_targets)} target(s)")
    print(SEP)
    for line in lines:
        print(f"  {line}" if line else "")
    print(f"\n  Skipped: {target_strs}")
    print(SEP)


def warn_link_type_skips(
    libraries: Dict[str, LibraryConfig],
    targets: List[BuildTarget],
) -> None:
    """Emit prominent warnings for any library/target combinations that will be
    silently skipped because the library's link_types restriction does not cover
    the requested link type, but the library *does* support the target platform.
    """
    for lib_name in sorted(libraries.keys()):
        lib = libraries[lib_name]
        if "all" in lib.link_types:
            continue
        skipped = [
            t
            for t in targets
            if lib.supports_platform(t.os.value)
            and not lib.supports_link_type(t.link_type.value)
        ]
        if skipped:
            _print_link_type_skip_warning(lib_name, skipped)


def find_unsatisfied_link_type_deps(
    libraries: Dict[str, LibraryConfig],
    targets: List[BuildTarget],
) -> List[str]:
    """Find dependencies no selected link type will build for their consumer.

    A library restricted to `link_types: [static]` is skipped for shared
    targets. That is fine while some selected target still produces its static
    build — the consumer links that (see
    DirectoryManager.get_dependency_dirs). It is fatal when nothing does: the
    dependency is simply never built, and the consumer's configure fails inside
    find_package, several layers away from the flag that caused it.
    """
    problems = []
    for name, lib in sorted(libraries.items()):
        for target in targets:
            if not lib.supports_platform(target.os.value):
                continue
            if not lib.supports_link_type(target.link_type.value):
                continue
            for dep_name in lib.dependencies:
                dep = libraries.get(dep_name)
                if dep is None or not dep.supports_platform(target.os.value):
                    continue
                if dep.supports_link_type(target.link_type.value):
                    continue
                # Skipped for this link type. Acceptable only if another
                # selected target builds it for the same platform and config.
                if any(
                    dep.supports_link_type(other.link_type.value)
                    for other in targets
                    if other.os == target.os
                    and other.arch == target.arch
                    and other.build_config == target.build_config
                ):
                    continue
                supported = ", ".join(dep.link_types)
                message = (
                    f"  - {name} is built as {target.link_type.value} but needs "
                    f"{dep_name}, which only supports {supported}; add "
                    f"--link {dep.link_types[0]} so it gets built"
                )
                if message not in problems:
                    problems.append(message)
    return problems


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Ocean Third-Party Build System",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )

    # Target selection
    parser.add_argument(
        "--target",
        "-t",
        type=str,
        action="append",
        help=(
            "Target platform(s). Accepts specific targets (e.g., ios_arm64, macos_arm64), "
            "OS group names (android, ios, macos, linux, win) to build all architectures "
            "for that OS, or 'all_supported' for everything the host can build. "
            "Comma-separated or multiple flags. "
            "Default: all platforms supported by the current host."
        ),
    )

    # Configuration
    parser.add_argument(
        "--config",
        "-c",
        type=str,
        action="append",
        default=None,
        help="Build config: debug, release. Comma-separated or multiple flags. Default: debug,release.",
    )
    parser.add_argument(
        "--link",
        "-l",
        type=str,
        action="append",
        default=None,
        help="Link type: static, shared. Comma-separated or multiple flags. "
        "Default: static,shared on Windows; static on other platforms.",
    )

    # Optional libraries
    parser.add_argument(
        "--with",
        dest="with_libs",
        type=str,
        action="append",
        default=[],
        help="Include optional library(ies). Can be specified multiple times. "
        "Comma-separated also supported. See --list-optional.",
    )
    parser.add_argument(
        "--with-group",
        type=str,
        action="append",
        default=[],
        help="Include all libraries in an optional group. Can be specified "
        "multiple times. Comma-separated also supported. See --list-optional.",
    )
    parser.add_argument(
        "--all",
        dest="build_all",
        action="store_true",
        help="Build all libraries including optional ones",
    )
    parser.add_argument(
        "--list-optional",
        action="store_true",
        help="List available optional libraries and exit",
    )

    # Specific library selection
    parser.add_argument(
        "--library",
        "-L",
        type=str,
        action="append",
        default=[],
        help="Build only specific library(ies) and their dependencies. "
        "Can be specified multiple times. Comma-separated also supported.",
    )

    # Parallelism
    parser.add_argument(
        "--parallel",
        "-j",
        type=_non_negative_int,
        default=0,
        help="Maximum parallel build jobs (default: auto-detect)",
    )
    parser.add_argument(
        "--jobs",
        type=int,
        default=0,
        help="Compile jobs per library (default: auto)",
    )

    # Other options
    parser.add_argument(
        "--manifest",
        "-m",
        type=str,
        default=None,
        help="Path to manifest file (default: dependencies.yaml)",
    )
    parser.add_argument(
        "--output-dir",
        "-o",
        type=str,
        default=None,
        help="Base output directory containing build/, install/, source/ subdirs "
        "(default: ${PWD}/ocean_3rdparty). Overridden by individual --build-dir, "
        "--install-dir, --source-dir flags.",
    )
    parser.add_argument(
        "--install-dir",
        type=str,
        default=None,
        help="Install directory for built libraries (default: <output-dir>/install)",
    )
    parser.add_argument(
        "--source-dir",
        type=str,
        default=None,
        help="Directory for cached source code (default: <output-dir>/source)",
    )
    parser.add_argument(
        "--build-dir",
        type=str,
        default=None,
        help="Directory for build artifacts (default: <output-dir>/build)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show build plan without building",
    )
    parser.add_argument(
        "--emit-expected-matrix",
        type=Path,
        default=None,
        help=(
            "Write the matrix of results audit_link_variants.py must find, "
            "then exit without building"
        ),
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Remove the source and build caches before building. The install "
        "tree is kept — delete it by hand, or use --output-dir, for a fully "
        "clean result.",
    )
    parser.add_argument(
        "--log-level",
        type=str,
        default="status",
        help="Log level: error, warning, status, verbose, debug (default: status)",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Shortcut for --log-level verbose",
    )
    parser.add_argument(
        "--with-cmake-configs",
        action="store_true",
        help=(
            "When --for-external-integration is in use, also copy CMake and "
            "pkg-config files into central locations under the install root. "
            "Has no effect in the standard (default) layout, which preserves "
            "configs natively in each library's prefix."
        ),
    )
    parser.add_argument(
        "--for-external-integration",
        action="store_true",
        help=(
            "Reorganize output for integration into non-CMake build systems "
            "(Visual Studio projects, Xcode, Bazel, custom Makefiles, IDE "
            "tooling). Produces a per-library layout with headers shared "
            "across architectures (<lib>/h/<platform>/) and libraries keyed "
            "by target (<lib>/lib/<target>/). The default standard layout "
            "places each library as a complete relocatable CMake install "
            "prefix under <install_root>/<target>/<library>/, which works "
            "directly with find_package(... CONFIG)."
        ),
    )
    parser.add_argument(
        "--skip-preflight",
        action="store_true",
        help="Skip pre-flight dependency checks",
    )
    parser.add_argument(
        "--vs-version",
        type=str,
        default=None,
        help="Visual Studio version to use (e.g., '2022', '2026'). "
        "Default: auto-detect newest installed version (checks 2026, 2022, 2019 in order). "
        "Within each version, checks Professional, Community, then Enterprise editions.",
    )
    parser.add_argument(
        "--android-api-level",
        type=int,
        default=None,
        help="Android API level for cross-compilation (e.g., 24, 32, 34). Default: 32.",
    )

    return parser.parse_args()


def parse_configs(config_args: Optional[List[str]]) -> List[BuildConfig]:
    """Parse config arguments (supports both comma-separated and multiple flags)."""
    if not config_args:
        return [BuildConfig.DEBUG, BuildConfig.RELEASE]

    configs = []
    for arg in config_args:
        for c in arg.split(","):
            c = c.strip().lower()
            if c == "debug":
                configs.append(BuildConfig.DEBUG)
            elif c == "release":
                configs.append(BuildConfig.RELEASE)
            else:
                raise ValueError(f"Unknown config: {c}")
    return _dedup(configs) or [BuildConfig.DEBUG, BuildConfig.RELEASE]


def parse_link_types(link_args: Optional[List[str]]) -> List[LinkType]:
    """Parse link type arguments (supports both comma-separated and multiple flags).

    On Windows, the default is both static and shared.
    On other platforms, the default is static only.
    """
    if not link_args:
        # On Windows, default to both static and shared
        if os.name == "nt":
            return [LinkType.STATIC, LinkType.SHARED]
        return [LinkType.STATIC]

    types = []
    for arg in link_args:
        for lt in arg.split(","):
            lt = lt.strip().lower()
            if lt == "static":
                types.append(LinkType.STATIC)
            elif lt == "shared":
                types.append(LinkType.SHARED)
            else:
                raise ValueError(f"Unknown link type: {lt}")

    # Fallback to defaults if no valid types parsed
    if not types:
        if os.name == "nt":
            return [LinkType.STATIC, LinkType.SHARED]
        return [LinkType.STATIC]

    return _dedup(types)


def find_unbuildable_windows_targets(
    platforms: Optional[List[tuple[OS, Arch]]],
    vs_version: Optional[str] = None,
) -> List[tuple[Arch, str]]:
    """Find requested Windows architectures the selected VS cannot build.

    Without this check, requesting an architecture whose MSVC tools are missing
    surfaces only as MSBuild's opaque 'The BaseOutputPath/OutputPath property is
    not set for project VCTargetsPath.vcxproj', deep into the configure step.

    Returns a list of (arch, reason) for the unbuildable architectures.
    """
    if not platforms or detect_host_os() != OS.WINDOWS:
        return []

    windows_archs = {arch for os_val, arch in platforms if os_val == OS.WINDOWS}
    if not windows_archs:
        return []

    installed_archs, scoped = detect_windows_archs(vs_version)
    if not scoped:
        # The arch list is a host-architecture guess, not an answer. Refusing
        # an explicitly requested target on that basis would leave a user whose
        # Visual Studio sits somewhere vswhere cannot see it with no way to
        # build at all. Let CMake report the real problem instead.
        return []

    installed = set(installed_archs)
    return [
        (
            arch,
            f"{WINDOWS_ARCH_COMPONENTS[arch]} not installed in the selected "
            "Visual Studio (add it via the Visual Studio Installer, or pass "
            "--vs-version to select another installation)",
        )
        for arch in sorted(windows_archs - installed, key=lambda a: a.value)
        if arch in WINDOWS_ARCH_COMPONENTS
    ]


class ParsedPlatforms(NamedTuple):
    """Platforms resolved from --target, and which of them the user named.

    `explicit` holds only the platforms named with an architecture. One that
    arrived by expanding a group (`win` -> win_x86_64 + win_arm64) or from
    `all_supported` is a suggestion rather than a request, so it is dropped
    with a warning when it cannot be built instead of failing the run.
    """

    platforms: List[tuple[OS, Arch]]
    explicit: Set[tuple[OS, Arch]]


def parse_platforms(
    target_args: Optional[List[str]],
    vs_version: Optional[str] = None,
) -> Optional[ParsedPlatforms]:
    """Parse platform arguments (supports both comma-separated and multiple flags).

    Special values:
        - "all_supported": Returns all platforms supported by the current host
        - Group names ("android", "ios", "macos", "linux", "win"):
          Expands to all architectures for that OS
    """
    if not target_args:
        return None

    platforms = []
    explicit: Set[tuple[OS, Arch]] = set()
    for arg in target_args:
        for t in arg.split(","):
            t = t.strip()
            if not t:
                continue
            if t.lower() == "all_supported":
                # Already filtered by host detection, so nothing here was
                # named by the user and nothing needs rejecting.
                return ParsedPlatforms(get_all_supported_platforms(vs_version)[0], set())
            if t.lower() in PLATFORM_GROUPS:
                platforms.extend(PLATFORM_GROUPS[t.lower()])
            else:
                platform = parse_platform_string(t)
                platforms.append(platform)
                explicit.add(platform)
    # De-duplicated because a group and a member of it are both documented
    # usage (`--target macos --target macos_arm64`): without this, the same
    # (library, target) job is submitted twice and two builds race in one
    # build directory.
    platforms = _dedup(platforms)
    return ParsedPlatforms(platforms, explicit) if platforms else None


def _report_install_tree_drift(
    dir_manager: DirectoryManager, known_libraries: set
) -> None:
    """Point out install-tree content the manifest can no longer explain.

    The tree is additive by design and --clean deliberately leaves it alone, so
    it is never pruned automatically — reporting is the honest middle ground.
    Only content that *no* invocation could produce is mentioned; output from a
    target or configuration you simply did not build this time is normal and
    stays quiet.
    """
    unknown = dir_manager.find_unrecognized_install_entries(known_libraries)
    mixed = dir_manager.find_mixed_install_layouts(known_libraries)
    if not unknown and not mixed:
        return

    print(f"\n{'─' * 70}")
    print("Note: the install tree contains entries this manifest cannot explain")
    print(f"{'─' * 70}")

    if unknown:
        print("  Not built by any library in the manifest (renamed or removed?):")
        for entry in unknown[:10]:
            print(f"    {entry}")
        if len(unknown) > 10:
            print(f"    ... and {len(unknown) - 10} more")
        print(
            "  Ocean's CMake puts every directory in the install tree on\n"
            "  CMAKE_PREFIX_PATH, so these are still visible to find_package."
        )

    if mixed:
        external, standard = mixed
        print("  Both install layouts are present in one tree:")
        print(f"    per-library (--for-external-integration): {external[0]}")
        print(f"    per-target (standard):                    {standard[0]}")
        print(
            "  Ocean's layout detection checks for a per-target directory "
            "first, so\n  the standard tree wins even if the other was built "
            "more recently."
        )

    print("  Nothing was deleted. Remove them by hand if they are stale.")


def _print_post_build_instructions(install_dir: Path) -> None:
    """Print next-step instructions after a successful 3rd-party build.

    Tells the user how to point downstream Ocean builds (CMake / Gradle apps
    such as the PointTracker demo) at the freshly built third-party install
    directory. Skipped for non-CMake layouts (see ``--for-external-integration``).
    """
    from lib.manifest import detect_windows_shell

    abs_install = install_dir.resolve()
    host_os = detect_host_os()

    print(f"\n{'═' * 70}")
    print("Next steps")
    print(f"{'═' * 70}")
    print(f"Third-party libraries installed at:\n  {abs_install}\n")
    print(
        "Set OCEAN_THIRDPARTY_PATH so the Ocean Gradle apps (e.g. the "
        "PointTracker demo) can find them:"
    )

    if host_os == OS.WINDOWS:
        # Both Windows PowerShell 5.x ("powershell") and PowerShell 7+ ("pwsh")
        # use the same $env: syntax to set environment variables in the current
        # session, so they share an output branch.
        shell = detect_windows_shell()
        ps_line = f'    $env:OCEAN_THIRDPARTY_PATH = "{abs_install}"'
        cmd_line = f"    set OCEAN_THIRDPARTY_PATH={abs_install}"
        if shell in ("pwsh", "powershell"):
            print("\n  PowerShell:")
            print(ps_line)
        elif shell == "cmd":
            print("\n  cmd.exe:")
            print(cmd_line)
        else:
            print("\n  PowerShell:")
            print(ps_line)
            print("\n  cmd.exe:")
            print(cmd_line)
    else:
        print("\n  bash / zsh:")
        print(f'    export OCEAN_THIRDPARTY_PATH="{abs_install}"')

    print(
        "\nAlternatively, pass the path explicitly to build_ocean.py:\n"
        f"  python build/python/build_ocean.py --third-party-dir {abs_install}"
    )
    print(f"{'═' * 70}")


def main() -> int:  # noqa: C901
    """Main entry point."""
    configure_console_encoding()

    args = parse_args()

    # Determine log level (--verbose is shortcut for --log-level verbose)
    if args.verbose:
        log_level = LogLevel.VERBOSE
    else:
        try:
            log_level = LogLevel.from_string(args.log_level)
        except ValueError as e:
            print(f"Error: {e}")
            return 1

    # Resolve the build matrix before any slow work happens, so a typo in
    # --target/--config/--link is an error message rather than a traceback
    # emerging after pre-flight and the manifest load have already printed.
    try:
        configs = parse_configs(args.config)
        link_types = parse_link_types(args.link)
        requested_platforms = parse_platforms(args.target, args.vs_version)
    except ValueError as e:
        print(f"Error: {e}")
        return 1

    # Without --target the platform set defaults to what this host supports,
    # which is the one thing an expected matrix must never be. The matrix is a
    # statement about the intended build, and its scope has to be stated.
    if args.emit_expected_matrix and not args.target:
        print(
            "Error: --emit-expected-matrix needs an explicit --target.\n"
            "  Without one the matrix would cover only the platforms this host\n"
            "  happens to support, and the gate reading it would silently stop\n"
            "  covering the rest."
        )
        return 1

    # Resolved once so the level the builder passes to CMake and the level
    # recorded in the build metadata cannot drift apart.
    android_api_level = args.android_api_level or DEFAULT_ANDROID_API_LEVEL

    # Run pre-flight checks (unless skipped, just listing, or only describing
    # the build). --emit-expected-matrix states what the build is supposed to
    # produce, which is a property of the manifest, not of this host.
    if (
        not args.skip_preflight
        and not args.list_optional
        and not args.dry_run
        and not args.emit_expected_matrix
    ):
        if not run_preflight_checks(log_level):
            return 1
        print()

    # Find manifest
    script_dir = Path(__file__).parent
    manifest_path = (
        Path(args.manifest) if args.manifest else script_dir / "dependencies.yaml"
    )

    if not manifest_path.exists():
        print(f"Error: Manifest not found: {manifest_path}")
        return 1

    # Load manifest
    print(f"Loading manifest: {manifest_path}")
    try:
        manifest = Manifest.from_file(manifest_path)
    except (OSError, ValueError) as e:
        print(f"Error: {e}")
        print("Run ./validate_manifest.py for a detailed schema check.")
        return 1
    print(f"  Found {len(manifest.libraries)} libraries")

    # Handle --list-optional
    if args.list_optional:
        print("\nOptional Libraries:")
        groups = manifest.get_optional_groups()
        for group, libs in sorted(groups.items()):
            print(f"\n  Group: {group}")
            for lib_name in libs:
                lib = manifest.libraries[lib_name]
                deps = (
                    f" (depends on: {', '.join(lib.dependencies)})"
                    if lib.dependencies
                    else ""
                )
                print(f"    {lib_name} ({lib.version}) - {lib.description}{deps}")

        # Ungrouped optional libs
        ungrouped = [
            name
            for name, lib in manifest.get_optional_libraries().items()
            if not lib.optional_group
        ]
        if ungrouped:
            print("\n  Ungrouped:")
            for lib_name in ungrouped:
                lib = manifest.libraries[lib_name]
                print(f"    {lib_name} ({lib.version}) - {lib.description}")
        return 0

    # Determine directories
    cwd = Path.cwd()
    base_dir = Path(args.output_dir) if args.output_dir else cwd / DEFAULT_BASE_DIR
    install_dir = Path(args.install_dir) if args.install_dir else base_dir / "install"
    source_dir = Path(args.source_dir) if args.source_dir else base_dir / "source"
    build_dir = Path(args.build_dir) if args.build_dir else base_dir / "build"

    # Initialize managers
    try:
        dir_manager = DirectoryManager(
            install_dir,
            source_dir,
            build_dir,
            for_external_integration=args.for_external_integration,
            create=not args.dry_run,
        )
    except OSError as e:
        print(f"Error: cannot create the output directories under {base_dir}: {e}")
        return 1
    fetcher = SourceFetcher(dir_manager, manifest_dir=manifest_path.parent)

    # Determine targets
    if requested_platforms is not None:
        platforms = requested_platforms.platforms
        # Skipped when emitting the matrix: this prunes Windows architectures
        # whose MSVC tools are missing *on this machine*, so `--target win
        # --emit-expected-matrix` on a host without the ARM64 tools would drop
        # win_arm64 from the matrix -- and the gate would then pass having never
        # covered it.
        unbuildable = (
            []
            if args.emit_expected_matrix
            else find_unbuildable_windows_targets(platforms, args.vs_version)
        )

        # Reject only what the user named by architecture.
        named = [
            (arch, reason)
            for arch, reason in unbuildable
            if (OS.WINDOWS, arch) in requested_platforms.explicit
        ]
        if named:
            print("Error: the selected Visual Studio cannot build these targets:")
            for arch, reason in named:
                print(f"  - win_{arch.value}: {reason}")
            return 1

        # Anything left came from expanding a group, so drop it with the same
        # warning a bare invocation prints. `--target win` is in the public
        # Windows build instructions and expands to arm64, which most Visual
        # Studio installs cannot build; failing a documented command there
        # helps nobody.
        if unbuildable:
            print("Skipped platforms:")
            for arch, reason in unbuildable:
                print(f"  - win_{arch.value}: {reason}")
            print()
            dropped = {(OS.WINDOWS, arch) for arch, _ in unbuildable}
            platforms = [p for p in platforms if p not in dropped]
    else:
        platforms, skipped_platforms = get_all_supported_platforms(args.vs_version)
        if skipped_platforms:
            print("Skipped platforms:")
            for group, reason in skipped_platforms:
                print(f"  - {group}: {reason}")
            print()

    # Determine MSVC toolset based on --vs-version (for Windows targets)
    msvc_toolset = None
    msvc_path = None
    if args.vs_version:
        msvc_toolset, msvc_path = get_msvc_toolset_version_and_path(args.vs_version)
    elif detect_host_os() == OS.WINDOWS:
        # Auto-detect if on Windows and no version specified
        msvc_toolset, msvc_path = get_msvc_toolset_version_and_path()

    # A (None, None) result for an explicit --vs-version means that version is
    # not installed. Continuing silently builds with the auto-detected toolset
    # and stamps *its* name into the output directories, so the user ends up
    # with vc143 artifacts in a tree they believe is vc142.
    if args.vs_version and detect_host_os() == OS.WINDOWS and msvc_toolset is None:
        print(f"Error: Visual Studio {args.vs_version} is not installed.")
        installed = get_all_installed_vs_versions()
        if installed:
            print("Detected installations:")
            for year, toolset, path in installed:
                print(f"  - {year} ({toolset}): {path}")
        else:
            print("  No Visual Studio installation was detected.")
        return 1

    # Expand platforms with configs/link types
    targets = [
        BuildTarget(
            os=os_val,
            arch=arch_val,
            build_config=config,
            link_type=link_type,
            msvc_toolset=msvc_toolset if os_val == OS.WINDOWS else None,
        )
        for os_val, arch_val in platforms
        for config in configs
        for link_type in link_types
    ]
    targets = _dedup(targets)

    # Filter out shared builds for platforms that don't support them.
    # iOS is excluded because of code-signing and Swift module issues.
    # Android shared builds are supported (see per-library link_types in
    # dependencies.yaml for libraries that must remain static).
    unsupported_shared = {OS.IOS}
    shared_skipped = any(
        t.link_type == LinkType.SHARED and t.os in unsupported_shared for t in targets
    )
    targets = [
        t
        for t in targets
        if not (t.link_type == LinkType.SHARED and t.os in unsupported_shared)
    ]
    if shared_skipped:
        print(
            "Note: Shared library builds are not supported for iOS. "
            "Skipping shared targets for iOS."
        )

    if not targets:
        print("Error: No valid targets remain after filtering.")
        return 1

    print(f"Targets: {', '.join(t.to_path_component() for t in targets)}")

    # Display Visual Studio version if any Windows targets are selected
    if any(t.os == OS.WINDOWS for t in targets) and msvc_toolset:
        # Map toolset back to VS year for user-friendly output
        toolset_to_year = {
            "vc141": "2017",
            "vc142": "2019",
            "vc143": "2022",
            "vc145": "2026",
        }
        vs_year = toolset_to_year.get(msvc_toolset, "unknown")
        print(f"Visual Studio: {vs_year} ({msvc_toolset})")
        if log_level >= LogLevel.VERBOSE:
            if msvc_path:
                print(f"  Path: {msvc_path}")
            # Show all detected Visual Studio versions
            all_vs_versions = get_all_installed_vs_versions()
            if all_vs_versions:
                print("  Detected installations:")
                for year, toolset, path in all_vs_versions:
                    selected_marker = " (selected)" if toolset == msvc_toolset else ""
                    print(f"    - {year} ({toolset}): {path}{selected_marker}")

    # Display Android NDK path if any Android targets are selected
    if any(t.os == OS.ANDROID for t in targets):
        from lib.platform import get_android_ndk_path

        ndk_path = get_android_ndk_path()
        if ndk_path:
            print(f"Android NDK: {ndk_path}")

    # Check toolchains for target platforms (unless skipped).
    #
    # Deliberately not run when emitting the expected matrix. This block drops
    # targets whose toolchain is missing, so generating the matrix here would
    # quietly omit every platform the generating host happens not to support --
    # and the gate that matrix feeds would then pass without ever covering them.
    # The matrix has to describe the intended build, not one machine's reach.
    if not args.skip_preflight and not args.dry_run and not args.emit_expected_matrix:
        target_strs = [t.to_path_component() for t in targets]
        all_available, available_target_strs = check_toolchains(target_strs, log_level)

        if not available_target_strs:
            # No targets available at all
            return 1

        # Filter targets to only those with available toolchains
        if not all_available:
            available_set = set(available_target_strs)
            targets = [t for t in targets if t.to_path_component() in available_set]
            print(f"Building for: {', '.join(t.to_path_component() for t in targets)}")

    # Printed only now that the target set is final: everything above can still
    # remove targets, and a command that advertises a set the build excluded
    # reproduces the wrong thing.
    print("\nEquivalent command:")
    print(f"  {get_equivalent_command(args, targets)}")
    print()

    # --with / --with-group take the same comma-separated form as --library, and
    # an unknown name is an error. Previously the value was matched verbatim
    # against library names, so `--with opencv,openssl` and `--with opencv2`
    # matched nothing, built the default set, and still printed success.
    with_libs = _split_list_arg(args.with_libs)
    with_groups = _split_list_arg(args.with_group)
    optional_libs = manifest.get_optional_libraries()
    optional_groups = manifest.get_optional_groups()

    for name in with_libs:
        if name not in manifest.libraries:
            print(f"Error: unknown library: {name}")
            print(
                f"Available optional libraries: {', '.join(sorted(optional_libs))} "
                "(see --list-optional)"
            )
            return 1
        if name not in optional_libs:
            print(f"Note: '{name}' is not optional; it is always built.")
    for name in with_groups:
        if name not in optional_groups:
            print(f"Error: unknown optional group: {name}")
            print(
                f"Available optional groups: {', '.join(sorted(optional_groups))} "
                "(see --list-optional)"
            )
            return 1

    # Validate --library up front, against the whole manifest
    requested_libs = _split_list_arg(args.library)
    for lib_name in requested_libs:
        if lib_name not in manifest.libraries:
            print(f"Error: Unknown library: {lib_name}")
            print(f"Available libraries: {', '.join(sorted(manifest.libraries.keys()))}")
            return 1

    # Filter libraries - get libraries that support ANY of the target platforms.
    # Naming a library with --library is a request for that library, so it joins
    # the optional allow-list: otherwise an optional target (opencv) was excluded
    # here and the --library step below could only intersect, never restore it.
    target_platforms = list({t.os.value for t in targets}) if targets else None
    libraries = manifest.filter_libraries(
        with_libs=with_libs + requested_libs,
        with_groups=with_groups,
        build_all=args.build_all,
        platforms=target_platforms,
    )

    # Handle --library flag: build only the requested libraries and their deps
    if requested_libs:
        # Build a graph to find all dependencies
        full_graph = DependencyGraph.from_manifest(manifest, manifest.libraries)

        # Collect all transitive dependencies
        needed_libs: set[str] = set(requested_libs)
        for lib_name in requested_libs:
            needed_libs.update(full_graph.get_all_dependencies(lib_name))

        # Filter libraries to only those needed
        libraries = {
            name: lib for name, lib in libraries.items() if name in needed_libs
        }

        # A requested library the platform filter removed used to vanish here,
        # leaving only its dependencies and a success banner at the end.
        unbuildable = [name for name in requested_libs if name not in libraries]
        if unbuildable:
            selected = ", ".join(sorted(target_platforms or []))
            print("Error: these libraries cannot be built for the selected targets:")
            for name in unbuildable:
                supported = ", ".join(manifest.libraries[name].platforms)
                print(
                    f"  - {name}: supports [{supported}], "
                    f"selected targets are [{selected}]"
                )
            return 1

        print(f"Requested: {', '.join(requested_libs)}")
        dependencies = sorted(set(libraries) - set(requested_libs))
        if dependencies:
            print(f"Including dependencies: {', '.join(dependencies)}")

    print(f"Libraries to build: {len(libraries)}")
    if not libraries:
        print("Error: no libraries are selected for the requested targets.")
        return 1

    if args.emit_expected_matrix:
        write_expected_matrix(args.emit_expected_matrix, libraries, targets)
        return 0

    # Handle --dry-run
    if args.dry_run:
        if args.clean:
            print(
                "Dry run: --clean skipped (it would delete the source and "
                "build caches)\n"
            )
        graph = DependencyGraph.from_manifest(manifest, libraries)
        print_build_plan(
            graph,
            [t.to_path_component() for t in targets],
            args.parallel or os.cpu_count() or 8,
            libraries=libraries,
        )
        return 0

    # The API level is recorded in the metadata rather than the path, so a
    # rebuild at a different level would silently leave a tree with some
    # libraries built for each. Refuse instead.
    conflicts = dir_manager.find_android_api_level_conflicts(
        libraries, targets, android_api_level
    )
    if conflicts:
        print(
            f"Error: {dir_manager.install_dir} already holds Android libraries "
            f"built for a different API level than the requested "
            f"android-{android_api_level}:"
        )
        for metadata_file, recorded in conflicts[:5]:
            print(f"  - {metadata_file.parent} was built for android-{recorded}")
        if len(conflicts) > 5:
            print(f"  ... and {len(conflicts) - 5} more")
        print(
            "  The API level is not part of the output path, so rebuilding here "
            "would leave\n  a tree with some libraries built for each. Remove the "
            "install directory and\n  re-run, or build elsewhere with --output-dir. "
            "(--clean only clears the source\n  and build caches, not the install "
            "tree.)"
        )
        return 1

    unsatisfied = find_unsatisfied_link_type_deps(libraries, targets)
    if unsatisfied:
        print("Error: some dependencies would never be built:")
        for problem in unsatisfied:
            print(problem)
        return 1

    # Handle --clean. Deliberately last: every validation above can still
    # refuse the run, and deleting a source cache the user then has to re-fetch
    # — only to be told the build was never going to start — is the worst
    # possible order. --dry-run returns before reaching here.
    if args.clean:
        # Each path is echoed only once it is actually gone: --source-dir and
        # --build-dir can point anywhere, this is the most destructive thing
        # the script does, and remove_tree can fail partway.
        print("Cleaning cache...")
        for cache_dir in (dir_manager.sources_dir, dir_manager.builds_dir):
            if cache_dir.exists():
                remove_tree(cache_dir)
                print(f"  removed: {cache_dir}")
            else:
                print(f"  absent:  {cache_dir}")
        print(f"  kept:    {dir_manager.install_dir}")
        print(
            "           --clean does not remove built output. Libraries this "
            "manifest no\n           longer builds stay there, and Ocean's CMake "
            "puts every directory in\n           the install tree on "
            "CMAKE_PREFIX_PATH — so a stale one can still be\n           picked "
            "up. Delete it by hand, or build elsewhere with --output-dir."
        )

    # Build!
    # Warn about any library/target combinations that will be silently skipped
    # due to link_types restrictions in the manifest.
    warn_link_type_skips(libraries, targets)

    try:
        build_all(
            manifest=manifest,
            libraries=libraries,
            targets=targets,
            dir_manager=dir_manager,
            fetcher=fetcher,
            max_parallel=args.parallel,
            jobs_per_lib=args.jobs,
            include_cmake_configs=args.with_cmake_configs,
            log_level=log_level,
            vs_version=args.vs_version,
            android_api_level=android_api_level,
        )
        print("\n✓ Build completed successfully!")
        _report_install_tree_drift(dir_manager, set(manifest.libraries))
        if not args.for_external_integration:
            _print_post_build_instructions(install_dir)
        return 0
    except KeyboardInterrupt:
        # Ctrl-C during a multi-hour first build is expected, not exceptional.
        # Reported as a normal outcome rather than a traceback, with 130 (the
        # shell convention for SIGINT) so wrapper scripts can tell it apart
        # from a build failure.
        print("\n✗ Interrupted. Partial results are kept; re-run to continue.")
        return 130
    except Exception as e:
        print(f"\n✗ Build failed: {e}")
        print(f"  Full output for each job: {build_dir}/<library>/<version>/<target>/build.log")
        if log_level >= LogLevel.VERBOSE:
            import traceback

            traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
