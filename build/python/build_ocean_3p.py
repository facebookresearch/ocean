#!/usr/bin/env fbpython
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

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
    # Build all required libraries for host platform (debug + release, static)
    ./build_ocean_3p.py

    # Build for specific target platform
    ./build_ocean_3p.py --target ios_arm64

    # Build for multiple platforms (both styles work)
    ./build_ocean_3p.py --target ios_arm64 --target macos_arm64
    ./build_ocean_3p.py --target ios_arm64,macos_arm64

    # Build for Android (requires ANDROID_NDK_HOME)
    ./build_ocean_3p.py --target android_arm64
    ./build_ocean_3p.py --target android_arm64,android_armv7

    # Build for all Android ABIs
    ./build_ocean_3p.py --target android_arm64,android_armv7,android_x86_64,android_x86

    # Build release only
    ./build_ocean_3p.py --config release

    # Build shared libraries
    ./build_ocean_3p.py --link shared

    # Build both static and shared (both styles work)
    ./build_ocean_3p.py --link static --link shared
    ./build_ocean_3p.py --link static,shared

    # Combining options
    ./build_ocean_3p.py -t ios_arm64,macos_arm64 -l static,shared -c release

    # Include optional libraries
    ./build_ocean_3p.py --with opencv --with openssl

    # Build specific library and its dependencies
    ./build_ocean_3p.py --library libpng
    ./build_ocean_3p.py -L libpng,freetype

    # Rebuild a single library (dependencies already built)
    ./build_ocean_3p.py --library curl --target ios_arm64

    # Show build plan without building
    ./build_ocean_3p.py --dry-run

    # Clean and rebuild
    ./build_ocean_3p.py --clean
"""

from __future__ import annotations

import argparse
import atexit
import os
import sys
import time
from concurrent.futures import as_completed, ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional

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
from lib.builder_base import BuildContext, Builder
from lib.platform import detect_host_os, get_msvc_toolset_version
from lib.progress import BuildPhase, ProgressDisplay


# ============================================================================
# Defaults
# ============================================================================

# Default output directories (relative to current working directory)
DEFAULT_BUILD_DIR = Path("ocean_build_thirdparty")
DEFAULT_INSTALL_DIR = Path("ocean_install_thirdparty")
DEFAULT_SOURCE_DIR = Path("ocean_source_thirdparty")


def get_default_platforms() -> List[tuple[OS, Arch]]:
    """Get default target platforms based on host OS.

    On macOS: builds for both iOS and macOS (arm64)
    On Linux: builds for host architecture
    On Windows: builds for host architecture
    """
    from lib.platform import detect_host_arch, detect_host_os

    host_os = detect_host_os()
    host_arch = detect_host_arch()

    if host_os == OS.MACOS:
        # On macOS, build for both iOS and macOS
        return [(OS.IOS, Arch.ARM64), (OS.MACOS, host_arch)]
    else:
        # On other platforms, build for host only
        return [(host_os, host_arch)]


def get_all_supported_platforms() -> List[tuple[OS, Arch]]:
    """Get all target platforms supported by the current host.

    Returns platforms that can be built on the current host, including
    cross-compilation targets when the required toolchains are available.

    On macOS:
        - macos_arm64 or macos_x86_64 (native)
        - ios_arm64 (if Xcode is available)
        - android_arm64, android_armv7, android_x86_64, android_x86 (if NDK available)

    On Linux:
        - linux_arm64 or linux_x86_64 (native)
        - android_arm64, android_armv7, android_x86_64, android_x86 (if NDK available)

    On Windows:
        - windows_x86_64, windows_x86, windows_arm64 (Visual Studio can cross-compile)
        - android_arm64, android_armv7, android_x86_64, android_x86 (if NDK available)
    """
    from lib.platform import (
        detect_host_arch,
        detect_host_os,
        get_android_ndk_path,
        get_ios_sdk_path,
    )

    platforms = []
    host_os = detect_host_os()
    host_arch = detect_host_arch()

    # Always include native host target
    platforms.append((host_os, host_arch))

    if host_os == OS.MACOS:
        # macOS can cross-compile to iOS if Xcode is available
        if get_ios_sdk_path("iphoneos"):
            platforms.append((OS.IOS, Arch.ARM64))

        # Also support x86_64 on arm64 Mac (and vice versa)
        if host_arch == Arch.ARM64:
            platforms.append((OS.MACOS, Arch.X86_64))
        elif host_arch == Arch.X86_64:
            platforms.append((OS.MACOS, Arch.ARM64))

    elif host_os == OS.WINDOWS:
        # Visual Studio can cross-compile to all Windows architectures
        # Add x86 and ARM64 targets (native target already added above)
        if host_arch != Arch.X86:
            platforms.append((OS.WINDOWS, Arch.X86))
        if host_arch != Arch.ARM64:
            platforms.append((OS.WINDOWS, Arch.ARM64))
        if host_arch != Arch.X86_64:
            platforms.append((OS.WINDOWS, Arch.X86_64))

    # All platforms can cross-compile to Android if NDK is available
    if get_android_ndk_path():
        platforms.extend(
            [
                (OS.ANDROID, Arch.ARM64),
                (OS.ANDROID, Arch.ARMV7),
                (OS.ANDROID, Arch.X86_64),
                (OS.ANDROID, Arch.X86),
            ]
        )

    return platforms


def get_equivalent_command(
    args: argparse.Namespace, script_name: str = "build_ocean_3p.py"
) -> str:
    """Generate the equivalent explicit command for the given args."""
    parts = [f"python {script_name}"]

    # Targets
    platforms = parse_platforms(args.target) if args.target else get_default_platforms()
    target_strs = [f"{os.value}_{arch.value}" for os, arch in platforms]
    parts.append(f"--target {','.join(target_strs)}")

    # Directories
    cwd = Path.cwd()
    install_dir = (
        Path(args.install_dir) if args.install_dir else cwd / DEFAULT_INSTALL_DIR
    )
    source_dir = Path(args.source_dir) if args.source_dir else cwd / DEFAULT_SOURCE_DIR
    build_dir = Path(args.build_dir) if args.build_dir else cwd / DEFAULT_BUILD_DIR
    parts.append(f"--install-dir {install_dir}")
    parts.append(f"--source-dir {source_dir}")
    parts.append(f"--build-dir {build_dir}")

    # Config
    configs = (
        parse_configs(args.config)
        if args.config
        else [BuildConfig.DEBUG, BuildConfig.RELEASE]
    )
    config_strs = [c.value for c in configs]
    parts.append(f"--config {','.join(config_strs)}")

    # Link type
    link_types = parse_link_types(args.link) if args.link else [LinkType.STATIC]
    link_strs = [lt.value for lt in link_types]
    parts.append(f"--link {','.join(link_strs)}")

    # Specific libraries (if provided)
    if hasattr(args, "library") and args.library:
        # Flatten all library arguments into a single list
        all_libs = []
        for arg in args.library:
            for lib in arg.split(","):
                lib = lib.strip()
                if lib:
                    all_libs.append(lib)
        if all_libs:
            parts.append(f"--library {','.join(all_libs)}")

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
    )

    # Get builder
    builder = get_builder(lib.build.system)

    # Build with progress reporting
    report_phase(BuildPhase.CONFIGURING)
    builder.configure(ctx)

    report_phase(BuildPhase.BUILDING)
    builder.build(ctx)

    report_phase(BuildPhase.INSTALLING)
    builder.install(ctx)

    # Reorganize output to final directory
    report_phase(BuildPhase.REORGANIZING)
    reorganize_output(
        paths.install_dir,
        paths.final_dir,
        target,
        lib.name,
        dir_manager.install_dir,
        include_cmake_configs,
    )

    # Write metadata
    source_info = {
        "type": lib.source.type,
        "url": lib.source.url,
        "ref": lib.source.ref,
        "fetched_commit": fetcher.get_actual_commit(lib.name, lib.version),
    }
    dir_manager.write_build_metadata(lib.name, lib.version, target, source_info)

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


def _normalize_debug_lib_name(filename: str) -> str:
    """Normalize a library filename for debug builds by ensuring 'd' suffix.

    Examples:
        libz.a -> libzd.a
        libcurl-d.a -> libcurld.a
        libfreetyped.a -> libfreetyped.a (already has d)
        libpng16d.a -> libpng16d.a (already has d)
    """
    # Library extensions we care about
    lib_extensions = {".a", ".dylib", ".so", ".lib", ".dll"}

    # Check if this is a library file
    for ext in lib_extensions:
        if filename.endswith(ext):
            base = filename[: -len(ext)]

            # Handle -d suffix (curl style) -> normalize to d
            if base.endswith("-d"):
                return base[:-2] + "d" + ext

            # Already has d suffix
            if base.endswith("d"):
                return filename

            # Add d suffix
            return base + "d" + ext

    # Handle versioned .so files (e.g., libz.so.1.3.1)
    if ".so." in filename:
        # Find the .so. part and add d before it
        idx = filename.index(".so.")
        base = filename[:idx]
        rest = filename[idx:]

        if base.endswith("-d"):
            return base[:-2] + "d" + rest
        if base.endswith("d"):
            return filename
        return base + "d" + rest

    return filename


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

    Only copies files with allowed header extensions.
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
            # Check if file has an allowed extension
            ext = path.suffix.lower()
            if ext not in ALLOWED_HEADER_EXTENSIONS:
                ignored.append(f)
        return ignored

    shutil.copytree(src_dir, dest_dir, ignore=ignore_unwanted)


def _copy_lib_files_recursive(
    src_dir: Path,
    dest_dir: Path,
    skip_extensions: set,
    skip_patterns: list,
    is_debug: bool = False,
    skip_versioned: bool = False,
) -> None:
    """Recursively copy library files from src_dir to dest_dir (flattened).

    Skips .framework directories and files matching skip_extensions/skip_patterns.
    All library files are copied directly to dest_dir regardless of subdirectory depth.
    For debug builds, normalizes library names to have 'd' suffix.
    For symlinks, copies the target file using the symlink's name (for CMake compatibility).
    If skip_versioned is True, skips versioned library files (e.g., libz.1.dylib).
    """
    import shutil

    for item in src_dir.iterdir():
        if item.is_dir() and not item.is_symlink():
            # Skip .framework directories
            if item.suffix == ".framework":
                continue
            # Recurse into other directories
            _copy_lib_files_recursive(
                item, dest_dir, skip_extensions, skip_patterns, is_debug, skip_versioned
            )
        elif item.is_file() or item.is_symlink():
            # For symlinks, we'll copy the target file using the symlink's name
            # This preserves CMake-expected names like libpng.dylib
            actual_file = item.resolve() if item.is_symlink() else item

            # Skip if the symlink target doesn't exist
            if not actual_file.exists():
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
                # Use the original name (symlink name if symlink, otherwise file name)
                # Apply debug suffix if needed
                dest_name = (
                    _normalize_debug_lib_name(item.name) if is_debug else item.name
                )
                dest = dest_dir / dest_name
                # Only copy if we don't already have this library
                if not dest.exists():
                    shutil.copy2(actual_file, dest)


def _create_debug_lib_symlinks(lib_dir: Path) -> None:
    """Create symlinks without 'd' suffix for debug shared libraries.

    CMake's Find modules (FindGIF, FindPNG, etc.) don't look for debug-suffixed
    library names like libgifd.dylib. This function creates symlinks like
    libgif.dylib -> libgifd.dylib so the Find modules can locate the libraries.

    Only creates symlinks for libraries where removing 'd' results in a different name.
    """
    lib_extensions = {".dylib", ".so"}

    for lib_file in lib_dir.iterdir():
        if not lib_file.is_file():
            continue

        name = lib_file.name
        ext = lib_file.suffix

        if ext not in lib_extensions:
            continue

        # Check if name ends with 'd' before the extension
        base = name[: -len(ext)]
        if not base.endswith("d"):
            continue

        # Create symlink without the 'd'
        non_debug_base = base[:-1]
        non_debug_name = non_debug_base + ext
        symlink_path = lib_dir / non_debug_name

        # Only create if it doesn't exist and differs from original
        if non_debug_name != name and not symlink_path.exists():
            symlink_path.symlink_to(lib_file.name)


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
    src_lib = install_dir / "lib"
    is_debug = target.build_config == BuildConfig.DEBUG
    is_shared = target.link_type == LinkType.SHARED

    if src_lib.exists():
        if final_lib.exists():
            shutil.rmtree(final_lib)
        final_lib.mkdir(parents=True, exist_ok=True)

        # Determine which library extensions to skip based on link type
        if target.link_type == LinkType.STATIC:
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

        for item in src_lib.iterdir():
            # Always skip cmake and pkgconfig in lib directory
            if item.is_dir() and item.name in ("cmake", "pkgconfig"):
                if include_cmake_configs:
                    # Copy to central location instead
                    central_dir = top_level_install_dir / item.name / library_name
                    if central_dir.exists():
                        shutil.rmtree(central_dir)
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
                    is_debug,
                    skip_versioned=is_shared,
                )
            elif item.is_file() or item.is_symlink():
                # For symlinks, we'll copy the target file using the symlink's name
                # This preserves CMake-expected names like libpng.dylib
                actual_file = item.resolve() if item.is_symlink() else item

                # Skip if the symlink target doesn't exist
                if not actual_file.exists():
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
                    # Use the original name (symlink name if symlink, otherwise file name)
                    # Apply debug suffix if needed
                    dest_name = (
                        _normalize_debug_lib_name(item.name) if is_debug else item.name
                    )
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
                    dest_name = (
                        _normalize_debug_lib_name(dll_file.name)
                        if is_debug
                        else dll_file.name
                    )
                    dest = final_lib / dest_name
                    if not dest.exists():
                        shutil.copy2(dll_file, dest)

    # For shared debug libraries, create symlinks without the 'd' suffix
    # so that CMake's Find modules can find them (they don't look for debug suffixes)
    if is_debug and target.link_type == LinkType.SHARED:
        if final_lib.exists():
            _create_debug_lib_symlinks(final_lib)

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
            level_idx, level_lib_count, level_target_count, len(jobs), level_libraries
        )

        # Execute all jobs in parallel
        with ThreadPoolExecutor(max_workers=min(max_parallel, len(jobs))) as executor:
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
                    raise

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
    """
    for lock_file in install_dir.glob("*/.build_metadata.lock"):
        try:
            lock_file.unlink()
        except OSError:
            pass  # Ignore errors (file may already be deleted)


# ============================================================================
# CLI
# ============================================================================


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
            "Target platform (e.g., ios_arm64, macos_arm64, android_arm64). "
            "Comma-separated or multiple flags. "
            "Use 'all_supported' to build for all platforms supported by the current host. "
            "Default on macOS: ios_arm64,macos_arm64. Default on other platforms: host. "
            "Android targets (android_arm64, android_armv7, android_x86_64, android_x86) "
            "require ANDROID_NDK_HOME to be set."
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
        help="Include optional library (can be specified multiple times)",
    )
    parser.add_argument(
        "--with-group",
        type=str,
        action="append",
        default=[],
        help="Include all libraries in an optional group",
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
        type=int,
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
        "--install-dir",
        type=str,
        default=None,
        help="Install directory for built libraries (default: ${PWD}/ocean_install_thirdparty)",
    )
    parser.add_argument(
        "--source-dir",
        type=str,
        default=None,
        help="Directory for cached source code (default: ${PWD}/ocean_source_thirdparty)",
    )
    parser.add_argument(
        "--build-dir",
        type=str,
        default=None,
        help="Directory for build artifacts (default: ${PWD}/ocean_build_thirdparty)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show build plan without building",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Clean cache before building",
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
        help="Include CMake and pkg-config files in output (disabled by default)",
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
        "Default: auto-detect latest installed version.",
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
    return configs or [BuildConfig.DEBUG, BuildConfig.RELEASE]


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

    return types


def parse_platforms(
    target_args: Optional[List[str]],
) -> Optional[List[tuple[OS, Arch]]]:
    """Parse platform arguments (supports both comma-separated and multiple flags).

    Special values:
        - "all_supported": Returns all platforms supported by the current host
    """
    if not target_args:
        return None

    platforms = []
    for arg in target_args:
        for t in arg.split(","):
            t = t.strip()
            if not t:
                continue
            if t.lower() == "all_supported":
                # Return all platforms supported by the current host
                return get_all_supported_platforms()
            platforms.append(parse_platform_string(t))
    return platforms or None


def main() -> int:  # noqa: C901
    """Main entry point."""
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

    # Run pre-flight checks (unless skipped or just listing)
    if not args.skip_preflight and not args.list_optional and not args.dry_run:
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
    manifest = Manifest.from_file(manifest_path)
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

    # Show equivalent command
    print("\nEquivalent command:")
    print(f"  {get_equivalent_command(args)}")
    print()

    # Determine directories (default: ${PWD}/ocean_{build,install,source}_thirdparty)
    cwd = Path.cwd()
    install_dir = (
        Path(args.install_dir) if args.install_dir else cwd / DEFAULT_INSTALL_DIR
    )
    source_dir = Path(args.source_dir) if args.source_dir else cwd / DEFAULT_SOURCE_DIR
    build_dir = Path(args.build_dir) if args.build_dir else cwd / DEFAULT_BUILD_DIR

    # Initialize managers
    dir_manager = DirectoryManager(install_dir, source_dir, build_dir)
    fetcher = SourceFetcher(dir_manager, manifest_dir=manifest_path.parent)

    # Handle --clean
    if args.clean:
        print("Cleaning cache...")
        dir_manager.clean_all()

    # Determine targets
    configs = parse_configs(args.config)
    link_types = parse_link_types(args.link)
    platforms = parse_platforms(args.target) if args.target else get_default_platforms()

    # Determine MSVC toolset based on --vs-version (for Windows targets)
    msvc_toolset = None
    if args.vs_version:
        msvc_toolset = get_msvc_toolset_version(args.vs_version)
    elif detect_host_os() == OS.WINDOWS:
        # Auto-detect if on Windows and no version specified
        msvc_toolset = get_msvc_toolset_version()

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

    # Filter out shared builds for platforms that don't support them
    unsupported_shared = {OS.ANDROID, OS.IOS}
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
            "Note: Shared library builds are not supported for Android/iOS. "
            "Skipping shared targets for those platforms."
        )

    if not targets:
        print("Error: No valid targets remain after filtering.")
        return 1

    print(f"Targets: {', '.join(t.to_path_component() for t in targets)}")

    # Check toolchains for target platforms (unless skipped)
    if not args.skip_preflight and not args.dry_run:
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

    # Filter libraries - get libraries that support ANY of the target platforms
    target_platforms = list({t.os.value for t in targets}) if targets else None
    libraries = manifest.filter_libraries(
        with_libs=args.with_libs,
        with_groups=args.with_group,
        build_all=args.build_all,
        platforms=target_platforms,
    )

    # Handle --library flag: filter to only specified libraries and their dependencies
    if args.library:
        requested_libs = []
        for arg in args.library:
            for lib in arg.split(","):
                lib = lib.strip()
                if lib:
                    requested_libs.append(lib)

        if requested_libs:
            # Validate that all requested libraries exist
            for lib_name in requested_libs:
                if lib_name not in manifest.libraries:
                    print(f"Error: Unknown library: {lib_name}")
                    print(
                        f"Available libraries: {', '.join(sorted(manifest.libraries.keys()))}"
                    )
                    return 1

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

            print(f"Requested: {', '.join(requested_libs)}")
            print(
                f"Including dependencies: {', '.join(sorted(needed_libs - set(requested_libs)))}"
            )

    print(f"Libraries to build: {len(libraries)}")

    # Handle --dry-run
    if args.dry_run:
        graph = DependencyGraph.from_manifest(manifest, libraries)
        print_build_plan(
            graph,
            [t.to_path_component() for t in targets],
            args.parallel or os.cpu_count() or 8,
            libraries=libraries,
        )
        return 0

    # Build!
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
            android_api_level=args.android_api_level,
        )
        print("\n✓ Build completed successfully!")
        return 0
    except Exception as e:
        print(f"\n✗ Build failed: {e}")
        if log_level >= LogLevel.VERBOSE:
            import traceback

            traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
