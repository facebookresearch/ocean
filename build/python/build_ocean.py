#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

"""Ocean Build System - Main Build Script.

This script builds Ocean using CMake with support for:
- Multiple target platforms (macOS, iOS, Linux, Android, Windows)
- Debug and release configurations
- Static and shared linking
- Both legacy CMake 3P layout and new Python 3P layout

Usage:
    # Build for host platform (debug + release, static)
    ./build_ocean.py

    # Build for specific target platform
    ./build_ocean.py --target ios_arm64

    # Build for multiple platforms
    ./build_ocean.py --target ios_arm64 --target macos_arm64
    ./build_ocean.py --target ios_arm64,macos_arm64

    # Build release only
    ./build_ocean.py --config release

    # Build shared libraries
    ./build_ocean.py --link shared

    # Use Python 3P layout (from build_ocean_3rdparty.py output)
    ./build_ocean.py --third-party-layout python --third-party-root ./3rdparty

    # Show build plan without building
    ./build_ocean.py --dry-run
"""

from __future__ import annotations

import argparse

# Add lib to path for imports - import platform module directly to avoid yaml dependency
import importlib.util
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import List, Optional, Tuple

_lib_dir = Path(__file__).parent / "lib"
_platform_spec = importlib.util.spec_from_file_location(
    "platform", _lib_dir / "platform.py"
)
_platform_module = importlib.util.module_from_spec(_platform_spec)
_platform_spec.loader.exec_module(_platform_module)

# Extract needed symbols
Arch = _platform_module.Arch
BuildConfig = _platform_module.BuildConfig
BuildTarget = _platform_module.BuildTarget
LinkType = _platform_module.LinkType
OS = _platform_module.OS
detect_host_arch = _platform_module.detect_host_arch
detect_host_os = _platform_module.detect_host_os
get_msvc_toolset_version = _platform_module.get_msvc_toolset_version
get_installed_windows_archs = _platform_module.get_installed_windows_archs
parse_platform_string = _platform_module.parse_platform_string


# ============================================================================
# Defaults
# ============================================================================

DEFAULT_BUILD_DIR = Path("ocean_build")
DEFAULT_INSTALL_DIR = Path("ocean_install")


def get_default_platforms() -> List[Tuple[OS, Arch]]:
    """Get default target platforms based on host OS."""
    host_os = detect_host_os()
    host_arch = detect_host_arch()
    return [(host_os, host_arch)]


def get_all_supported_platforms() -> List[Tuple[OS, Arch]]:
    """Get all target platforms supported by the current host.

    Returns platforms that can be built on the current host, including
    cross-compilation targets when the required toolchains are available.
    """
    # Import additional functions needed for platform detection
    _platform_spec = importlib.util.spec_from_file_location(
        "platform", Path(__file__).parent / "lib" / "platform.py"
    )
    _platform_mod = importlib.util.module_from_spec(_platform_spec)
    _platform_spec.loader.exec_module(_platform_mod)

    get_android_ndk_path = _platform_mod.get_android_ndk_path
    get_ios_sdk_path = _platform_mod.get_ios_sdk_path

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
        # Add all 64-bit architectures that have MSVC tools installed.
        for arch in get_installed_windows_archs():
            if (OS.WINDOWS, arch) not in platforms:
                platforms.append((OS.WINDOWS, arch))

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


# ============================================================================
# CMake Invocation
# ============================================================================


def get_cmake_preset_name(target: BuildTarget, quest_mode: bool = False) -> str:
    """Get the CMake preset name for a build target."""
    if quest_mode and target.os == OS.ANDROID:
        return f"android-quest-{target.link_type.value}-{target.build_config.value}"

    # Map our arch names to preset arch names
    arch_map = {
        Arch.ARM64: "arm64",
        Arch.ARMV7: "arm32",
        Arch.X86_64: "x64",
        Arch.X86: "x86",
    }
    arch_str = arch_map.get(target.arch, target.arch.value)

    return f"{target.os.value}-{arch_str}-{target.link_type.value}-{target.build_config.value}"


def get_third_party_paths_cmake_layout(
    third_party_dir: Path,
    target: BuildTarget,
) -> List[Path]:
    """Get CMAKE_PREFIX_PATH entries for legacy CMake 3P layout.

    Layout: <root>/<platform>/<arch>_<link>_<config>/[<lib>/]
    """
    # Map our names to legacy directory names
    platform = target.os.value
    arch_map = {
        Arch.ARM64: "arm64",
        Arch.ARMV7: "arm32",
        Arch.X86_64: "x64",
        Arch.X86: "x86",
    }
    arch = arch_map.get(target.arch, target.arch.value)
    link = target.link_type.value
    config = target.build_config.value

    target_dir = third_party_dir / platform / f"{arch}_{link}_{config}"

    if not target_dir.exists():
        return []

    # Check for subdivided structure (per-library directories)
    subdirs = [d for d in target_dir.iterdir() if d.is_dir()]
    known_libs = {"zlib", "eigen", "libpng", "freetype", "curl"}

    if any(d.name in known_libs for d in subdirs):
        # Subdivided: return each library directory
        return [d for d in subdirs if not d.name.startswith(".")]
    else:
        # Flat: return the target directory itself
        return [target_dir]


def get_third_party_paths_python_layout(
    third_party_root: Path,
    target: BuildTarget,
) -> Tuple[List[Path], List[Path], List[Path]]:
    """Get search paths for Python 3P layout.

    Layout:
        <root>/<lib>/h/<platform>/     (headers)
        <root>/<lib>/lib/<target>/     (libraries)

    Some libraries install headers in version subdirectories (e.g., wxWidgets
    installs to h/<platform>/wx-3.3/). We detect these and add them to the
    include path.

    Some libraries also need the lib directory on the include path for
    per-configuration headers (e.g., wxWidgets setup.h).

    Returns:
        Tuple of (prefix_paths, include_paths, library_paths)
    """
    platform = target.os.value
    target_str = target.to_path_component()

    prefix_paths = []
    include_paths = []
    library_paths = []

    if not third_party_root.exists():
        return prefix_paths, include_paths, library_paths

    # Libraries that need the lib directory on the include path for setup.h
    libs_with_config_headers = {"wxwidgets"}

    for lib_dir in third_party_root.iterdir():
        if not lib_dir.is_dir() or lib_dir.name.startswith("."):
            continue

        lib_name = lib_dir.name

        # Header path: <lib>/h/<platform>/
        header_path = lib_dir / "h" / platform
        if header_path.exists():
            # Check for version subdirectories (e.g., wx-3.3/)
            # Some libraries install to include/<version>/ which becomes h/<platform>/<version>/
            subdirs = [d for d in header_path.iterdir() if d.is_dir()]
            if subdirs and len(subdirs) == 1:
                # Single subdirectory - likely a version directory
                # Check if it contains typical header files/dirs
                subdir = subdirs[0]
                subdir_contents = list(subdir.iterdir())
                has_headers = any(
                    (f.suffix in {".h", ".hpp"} or f.is_dir()) for f in subdir_contents
                )
                if has_headers:
                    # Add the version subdirectory to include path
                    include_paths.append(subdir)
                else:
                    include_paths.append(header_path)
            else:
                include_paths.append(header_path)

        # Library path: <lib>/lib/<target>/
        lib_path = lib_dir / "lib" / target_str
        if lib_path.exists():
            library_paths.append(lib_path)
            prefix_paths.append(lib_dir)

            # Some libraries need the lib directory on the include path
            # for per-configuration headers (e.g., wxWidgets setup.h)
            if lib_name in libs_with_config_headers:
                include_paths.append(lib_path)

    return prefix_paths, include_paths, library_paths


def run_cmake_build(
    target: BuildTarget,
    ocean_source_dir: Path,
    build_dir: Path,
    install_dir: Path,
    third_party_layout: str,
    third_party_dir: Optional[Path],
    quest_mode: bool = False,
    minimal: bool = False,
    configure_only: bool = False,
    generator: Optional[str] = None,
    log_level: str = "ERROR",
    android_sdk: str = "android-32",
) -> bool:
    """Run CMake configure and build for a single target.

    Returns:
        True if successful, False otherwise.
    """
    preset_name = get_cmake_preset_name(target, quest_mode)
    target_str = target.to_path_component()

    # Determine build and install directories for this configuration
    if quest_mode and target.os == OS.ANDROID:
        config_suffix = f"quest_{target.link_type.value}_{target.build_config.value}"
    else:
        config_suffix = target_str

    config_build_dir = build_dir / config_suffix
    config_install_dir = install_dir / config_suffix

    # Build CMAKE_PREFIX_PATH based on layout
    cmake_prefix_path = []
    cmake_include_path = []
    cmake_library_path = []

    if third_party_dir:
        if third_party_layout == "python":
            prefix, includes, libs = get_third_party_paths_python_layout(
                third_party_dir, target
            )
            cmake_prefix_path = prefix
            cmake_include_path = includes
            cmake_library_path = libs
        else:
            cmake_prefix_path = get_third_party_paths_cmake_layout(
                third_party_dir, target
            )

    # Configure arguments
    configure_args = [
        "cmake",
        "--preset",
        preset_name,
        f"--log-level={log_level}",
        "-B",
        str(config_build_dir),
        f"-DCMAKE_INSTALL_PREFIX={config_install_dir}",
    ]

    if cmake_prefix_path:
        configure_args.append(
            f"-DCMAKE_PREFIX_PATH={';'.join(str(p) for p in cmake_prefix_path)}"
        )

    if third_party_layout == "python" and third_party_dir:
        configure_args.append("-DOCEAN_THIRD_PARTY_LAYOUT=python")
        configure_args.append(f"-DOCEAN_THIRD_PARTY_ROOT={third_party_dir}")

    if cmake_include_path:
        configure_args.append(
            f"-DCMAKE_INCLUDE_PATH={';'.join(str(p) for p in cmake_include_path)}"
        )

    if cmake_library_path:
        configure_args.append(
            f"-DCMAKE_LIBRARY_PATH={';'.join(str(p) for p in cmake_library_path)}"
        )

    if generator:
        configure_args.extend(["-G", generator])

    if target.os == OS.ANDROID:
        configure_args.append(
            f"-DCMAKE_FIND_ROOT_PATH={';'.join(str(p) for p in cmake_prefix_path)}"
        )
        configure_args.append(f"-DANDROID_PLATFORM={android_sdk}")

    if minimal:
        configure_args.extend(
            [
                "-DOCEAN_BUILD_MINIMAL=ON",
                "-DOCEAN_BUILD_DEMOS=OFF",
                "-DOCEAN_BUILD_TESTS=OFF",
            ]
        )

    # Run configure
    print(f"\n{'=' * 60}")
    print(f"Configuring: {preset_name}")
    print(f"{'=' * 60}\n")

    try:
        subprocess.run(
            configure_args,
            cwd=ocean_source_dir,
            check=True,
        )
    except subprocess.CalledProcessError as e:
        print(f"Configure failed: {e}")
        return False

    if configure_only:
        print("Configure-only mode: skipping build step.")
        return True

    # Build arguments
    build_args = [
        "cmake",
        "--build",
        str(config_build_dir),
        "--target",
        "install",
        "-j",
    ]

    # Add config for multi-config generators
    if target.os in (OS.IOS, OS.WINDOWS):
        build_args.extend(["--config", target.build_config.value.capitalize()])

    # Run build
    print(f"\n{'=' * 60}")
    print(f"Building: {preset_name}")
    print(f"{'=' * 60}\n")

    try:
        subprocess.run(build_args, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Build failed: {e}")
        return False

    return True


# ============================================================================
# CLI
# ============================================================================


def parse_args() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Ocean Build System",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )

    # Target selection
    parser.add_argument(
        "--target",
        "-t",
        type=str,
        action="append",
        help="Target platform (e.g., ios_arm64, macos_arm64, android_arm64). "
        "Comma-separated or multiple flags. "
        "Use 'all_supported' to build for all platforms supported by the current host. "
        "Default: host platform.",
    )

    # Configuration
    parser.add_argument(
        "--config",
        "-c",
        type=str,
        action="append",
        default=None,
        help="Build config: debug, release. Comma-separated or multiple flags. "
        "Default: debug,release.",
    )
    parser.add_argument(
        "--link",
        "-l",
        type=str,
        action="append",
        default=None,
        help="Link type: static, shared. Comma-separated or multiple flags. "
        "Default: static.",
    )

    # Paths
    parser.add_argument(
        "--build-dir",
        "-b",
        type=str,
        default=None,
        help=f"Build directory. Default: ${{PWD}}/{DEFAULT_BUILD_DIR}",
    )
    parser.add_argument(
        "--install-dir",
        "-i",
        type=str,
        default=None,
        help=f"Install directory. Default: ${{PWD}}/{DEFAULT_INSTALL_DIR}",
    )
    parser.add_argument(
        "--third-party-dir",
        type=str,
        default=None,
        help="Third-party libraries directory. "
        "Default: ocean_install_thirdparty (python layout) or bin/cmake/3rdparty (cmake layout)",
    )

    # Third-party layout
    parser.add_argument(
        "--third-party-layout",
        type=str,
        choices=["cmake", "python"],
        default="cmake",
        help="Third-party library layout: 'cmake' (legacy) or 'python' (new). "
        "Default: cmake.",
    )

    # Build options
    parser.add_argument(
        "--quest",
        "-q",
        action="store_true",
        help="Build for Meta Quest (Android ARM64 with Quest extensions)",
    )
    parser.add_argument(
        "--minimal",
        "-m",
        action="store_true",
        help="Enable minimal build (base, cv, math only)",
    )
    parser.add_argument(
        "--generator",
        "-g",
        type=str,
        default=None,
        help="CMake generator to use (overrides preset default)",
    )
    parser.add_argument(
        "--log-level",
        type=str,
        default="ERROR",
        help="CMake log level: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE. "
        "Default: ERROR",
    )
    parser.add_argument(
        "--android-sdk",
        type=str,
        default="android-32",
        help="Android SDK version. Default: android-32",
    )
    parser.add_argument(
        "--vs-version",
        type=str,
        default=None,
        help="Visual Studio version to use (e.g., '2022', '2026'). "
        "Default: auto-detect latest installed version.",
    )

    # Build control
    parser.add_argument(
        "--configure-only",
        action="store_true",
        help="Only run CMake configure step, skip build",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show build plan without building",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Shortcut for --log-level STATUS",
    )

    return parser.parse_args()


def parse_configs(config_args: Optional[List[str]]) -> List[BuildConfig]:
    """Parse config arguments."""
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
    """Parse link type arguments."""
    if not link_args:
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
    return types or [LinkType.STATIC]


def parse_platforms(
    target_args: Optional[List[str]],
) -> Optional[List[Tuple[OS, Arch]]]:
    """Parse platform arguments.

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


def main() -> int:
    """Main entry point."""
    args = parse_args()

    # Determine log level
    if args.verbose:
        log_level = "STATUS"
    else:
        log_level = args.log_level.upper()

    # Validate log level
    valid_levels = {"ERROR", "WARNING", "NOTICE", "STATUS", "VERBOSE", "DEBUG", "TRACE"}
    if log_level not in valid_levels:
        print(f"Error: Invalid log level: {log_level}")
        return 1

    # Find Ocean source directory
    script_dir = Path(__file__).parent
    ocean_source_dir = (script_dir / "../..").resolve()

    if not (ocean_source_dir / "CMakeLists.txt").exists():
        print(f"Error: Cannot find Ocean source at {ocean_source_dir}")
        return 1

    # Determine directories
    cwd = Path.cwd()
    build_dir = Path(args.build_dir) if args.build_dir else cwd / DEFAULT_BUILD_DIR
    install_dir = (
        Path(args.install_dir) if args.install_dir else cwd / DEFAULT_INSTALL_DIR
    )

    # Default third-party directory depends on layout
    if args.third_party_dir:
        third_party_dir = Path(args.third_party_dir)
    elif args.third_party_layout == "python":
        # Python layout: default to ocean_install_thirdparty (from build_ocean_3rdparty.py)
        third_party_dir = cwd / "ocean_install_thirdparty"
    else:
        # CMake layout: default to bin/cmake/3rdparty
        third_party_dir = cwd / "bin" / "cmake" / "3rdparty"

    # Parse build configuration
    configs = parse_configs(args.config)
    link_types = parse_link_types(args.link)
    platforms = parse_platforms(args.target) if args.target else get_default_platforms()

    # Handle Quest mode
    if args.quest:
        platforms = [(OS.ANDROID, Arch.ARM64)]

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

    # Print build plan
    print("\nOcean Build Configuration")
    print("=" * 60)
    print(f"  Source dir:         {ocean_source_dir}")
    print(f"  Build dir:          {build_dir}")
    print(f"  Install dir:        {install_dir}")
    print(f"  Third-party dir:    {third_party_dir}")
    print(f"  Third-party layout: {args.third_party_layout}")
    print(f"  Log level:          {log_level}")
    if args.quest:
        print("  Quest mode:         enabled")
    if args.minimal:
        print("  Minimal build:      enabled")
    print(f"\n  Targets ({len(targets)}):")
    for t in targets:
        preset = get_cmake_preset_name(t, args.quest)
        print(f"    - {preset}")
    print()

    if args.dry_run:
        print("Dry run: exiting without building.")
        return 0

    # Check third-party directory
    if args.third_party_layout == "python":
        if not third_party_dir.exists():
            print(f"Error: Third-party directory not found: {third_party_dir}")
            print("  Build third-party libraries first with build_ocean_3rdparty.py")
            return 1
    elif not third_party_dir.exists():
        print(f"Warning: Third-party directory not found: {third_party_dir}")
        print("  Build may fail if dependencies are not in system paths.")

    # Build each target
    failed_builds = []
    for target in targets:
        success = run_cmake_build(
            target=target,
            ocean_source_dir=ocean_source_dir,
            build_dir=build_dir,
            install_dir=install_dir,
            third_party_layout=args.third_party_layout,
            third_party_dir=third_party_dir,
            quest_mode=args.quest,
            minimal=args.minimal,
            configure_only=args.configure_only,
            generator=args.generator,
            log_level=log_level,
            android_sdk=args.android_sdk,
        )
        if not success:
            failed_builds.append(get_cmake_preset_name(target, args.quest))

    # Report results
    print(f"\n{'=' * 60}")
    print("Build Summary")
    print(f"{'=' * 60}")

    if failed_builds:
        print(f"\nFailed builds ({len(failed_builds)}):")
        for build in failed_builds:
            print(f"  - {build}")
        return 1
    else:
        print("\nAll builds completed successfully.")
        print("\nInstall locations:")
        for target in targets:
            if args.quest and target.os == OS.ANDROID:
                suffix = f"quest_{target.link_type.value}_{target.build_config.value}"
            else:
                suffix = target.to_path_component()
            print(f"  - {install_dir / target.os.value / suffix}")
        return 0


if __name__ == "__main__":
    sys.exit(main())
