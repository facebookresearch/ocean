# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Platform detection and build target definitions."""

from __future__ import annotations

import os
import platform
import subprocess
from dataclasses import dataclass
from enum import Enum
from typing import List, Optional


class OS(Enum):
    """Target operating system."""

    MACOS = "macos"
    IOS = "ios"
    LINUX = "linux"
    ANDROID = "android"
    WINDOWS = "windows"


class Arch(Enum):
    """Target CPU architecture."""

    ARM64 = "arm64"
    X86_64 = "x86_64"
    ARMV7 = "armv7"
    X86 = "x86"


class BuildConfig(Enum):
    """Build configuration (debug/release)."""

    DEBUG = "debug"
    RELEASE = "release"


class LinkType(Enum):
    """Library linking type."""

    STATIC = "static"
    SHARED = "shared"


def get_msvc_toolset_version(vs_version: Optional[str] = None) -> Optional[str]:
    """Get the MSVC toolset version.

    Args:
        vs_version: Optional Visual Studio version year (e.g., "2022", "2026").
                   If specified, returns the toolset for that version.
                   If not specified, auto-detects the latest installed version.

    Returns toolset version string like 'vc143' (VS2022), 'vc145' (VS2026), or 'vc142' (VS2019).
    Returns None if not on Windows or MSVC is not available.
    """
    if platform.system().lower() != "windows":
        return None

    # Map VS year to toolset version
    # VS 2017 = vc141, VS 2019 = vc142, VS 2022 = vc143, VS 2026 = vc145
    year_to_toolset = {
        "2017": "vc141",
        "2019": "vc142",
        "2022": "vc143",
        "2026": "vc145",
    }

    # If a specific version is requested, return its toolset
    if vs_version and vs_version in year_to_toolset:
        return year_to_toolset[vs_version]

    # Auto-detect: check for installed Visual Studio versions (newest first)
    vs_checks = [
        # VS 2026 (vc145)
        (
            "vc145",
            [
                os.environ.get("VS180COMNTOOLS"),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2026\Professional\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2026\Community\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2026\Enterprise\VC"
                ),
            ],
        ),
        # VS 2022 (vc143)
        (
            "vc143",
            [
                os.environ.get("VS170COMNTOOLS"),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC"
                ),
            ],
        ),
        # VS 2019 (vc142)
        (
            "vc142",
            [
                os.environ.get("VS160COMNTOOLS"),
                os.path.expandvars(
                    r"%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC"
                ),
                os.path.expandvars(
                    r"%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC"
                ),
            ],
        ),
    ]

    for toolset, paths in vs_checks:
        for path in paths:
            if path and os.path.exists(path):
                return toolset

    # Fallback: try to detect from cl.exe version
    try:
        result = subprocess.run(
            ["cl"],
            capture_output=True,
            text=True,
        )
        # cl.exe outputs version info to stderr
        version_output = result.stderr
        if "19.5" in version_output:  # VS2026 18.x (estimated)
            return "vc145"
        elif "19.4" in version_output:  # VS2022 17.x
            return "vc143"
        elif "19.3" in version_output:  # VS2022 17.0-17.3
            return "vc143"
        elif "19.2" in version_output:  # VS2019
            return "vc142"
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    # Default to vc143 (VS2022) if we can't detect
    return "vc143"


@dataclass(frozen=True)
class BuildTarget:
    """Represents a specific build target configuration."""

    os: OS
    arch: Arch
    build_config: BuildConfig = BuildConfig.RELEASE
    link_type: LinkType = LinkType.STATIC
    msvc_toolset: Optional[str] = None  # e.g., "vc143" for VS2022

    def to_path_component(self) -> str:
        """Convert to directory name component.

        Examples:
            macos_arm64_static
            macos_arm64_static_debug
            ios_arm64_static
            android_arm64_static_debug
            windows_x86_64_vc143_static
            windows_x86_64_vc143_static_debug
        """
        parts = [self.os.value, self.arch.value]
        # Include MSVC toolset version for Windows
        if self.os == OS.WINDOWS:
            toolset = self.msvc_toolset or get_msvc_toolset_version() or "vc143"
            parts.append(toolset)
        parts.append(self.link_type.value)
        if self.build_config == BuildConfig.DEBUG:
            parts.append("debug")
        return "_".join(parts)

    def to_platform_component(self) -> str:
        """Convert to platform-only directory name (OS only).

        Used for headers which don't differ between architectures,
        debug/release, or static/shared.

        Examples:
            macos
            ios
            android
        """
        return self.os.value

    def __str__(self) -> str:
        return self.to_path_component()


def detect_host_os() -> OS:
    """Detect the host operating system."""
    system = platform.system().lower()
    if system == "darwin":
        return OS.MACOS
    elif system == "linux":
        return OS.LINUX
    elif system == "windows":
        return OS.WINDOWS
    else:
        raise RuntimeError(f"Unsupported host OS: {system}")


def detect_host_arch() -> Arch:
    """Detect the host CPU architecture."""
    machine = platform.machine().lower()
    if machine in ("arm64", "aarch64"):
        return Arch.ARM64
    elif machine in ("x86_64", "amd64"):
        return Arch.X86_64
    elif machine in ("armv7", "armv7l"):
        return Arch.ARMV7
    elif machine in ("i386", "i686", "x86"):
        return Arch.X86
    else:
        raise RuntimeError(f"Unsupported host architecture: {machine}")


def detect_host_target(
    build_config: BuildConfig = BuildConfig.RELEASE,
    link_type: LinkType = LinkType.STATIC,
) -> BuildTarget:
    """Detect build target for the host machine."""
    return BuildTarget(
        os=detect_host_os(),
        arch=detect_host_arch(),
        build_config=build_config,
        link_type=link_type,
    )


def get_android_ndk_path() -> Optional[str]:
    """Get Android NDK path from environment or common locations.

    Checks the following environment variables (in order):
    - ANDROID_NDK_HOME (official recommended name)
    - ANDROID_NDK (common shorthand)
    - NDK_HOME (alternative)
    - ANDROID_NDK_ROOT (used in some documentation)
    - NDK_ROOT (used in some build systems)

    Then checks common installation paths:
    - ~/Library/Android/sdk/ndk/ (macOS default)
    - ~/Android/Sdk/ndk/ (Linux default)
    - /opt/android-ndk/
    """
    # Check environment variables (in order of preference)
    env_vars = [
        "ANDROID_NDK_HOME",
        "ANDROID_NDK",
        "NDK_HOME",
        "ANDROID_NDK_ROOT",
        "NDK_ROOT",
    ]

    for var in env_vars:
        ndk_path = os.environ.get(var)
        if ndk_path and os.path.isdir(ndk_path):
            return ndk_path

    # Check common locations
    common_paths = [
        os.path.expanduser("~/Library/Android/sdk/ndk"),  # macOS
        os.path.expanduser("~/Android/Sdk/ndk"),  # Linux
        "/opt/android-ndk",
    ]

    for base_path in common_paths:
        if os.path.isdir(base_path):
            # Get the latest NDK version
            try:
                versions = sorted(os.listdir(base_path), reverse=True)
                if versions:
                    return os.path.join(base_path, versions[0])
            except OSError:
                continue

    return None


def get_ios_sdk_path(sdk_type: str = "iphoneos") -> Optional[str]:
    """Get iOS SDK path using xcrun."""
    try:
        result = subprocess.run(
            ["xcrun", "--sdk", sdk_type, "--show-sdk-path"],
            capture_output=True,
            text=True,
            check=True,
        )
        return result.stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None


def is_cross_compile(target: BuildTarget) -> bool:
    """Check if building for this target requires cross-compilation."""
    host_os = detect_host_os()
    host_arch = detect_host_arch()

    # Cross-compiling to different OS
    if target.os == OS.ANDROID:
        return True
    if target.os == OS.IOS:
        return True
    if target.os != host_os:
        return True

    # Cross-compiling to different architecture on same OS
    if target.arch != host_arch:
        return True

    return False


def find_ninja_program() -> Optional[str]:
    """Find the Ninja build tool executable.

    Checks:
    1. System PATH
    2. Ninja bundled with Android SDK CMake (ANDROID_HOME/cmake/*/bin/)
    3. Ninja in NDK prebuilt (ANDROID_NDK/prebuilt/*/bin/)

    Returns:
        Path to ninja executable if found, None otherwise.
    """
    # Check PATH first
    try:
        subprocess.run(
            ["ninja", "--version"],
            capture_output=True,
            check=True,
        )
        return "ninja"
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    if platform.system().lower() == "windows":
        # Check Ninja bundled with the Android SDK/CMake
        android_home = os.environ.get("ANDROID_HOME")
        if android_home:
            cmake_dir = os.path.join(android_home, "cmake")
            if os.path.isdir(cmake_dir):
                for version_dir in sorted(os.listdir(cmake_dir), reverse=True):
                    ninja_path = os.path.join(
                        cmake_dir, version_dir, "bin", "ninja.exe"
                    )
                    if os.path.exists(ninja_path):
                        return ninja_path

        # Check NDK prebuilt directory
        ndk_path = get_android_ndk_path()
        if ndk_path:
            prebuilt_dir = os.path.join(ndk_path, "prebuilt")
            if os.path.isdir(prebuilt_dir):
                for host_dir in os.listdir(prebuilt_dir):
                    ninja_path = os.path.join(
                        prebuilt_dir, host_dir, "bin", "ninja.exe"
                    )
                    if os.path.exists(ninja_path):
                        return ninja_path

    return None


def find_make_program() -> Optional[str]:
    """Find the Make build tool executable.

    Checks:
    1. System PATH
    2. Make bundled with NDK (ANDROID_NDK/prebuilt/*/bin/)

    Returns:
        Path to make executable if found, None otherwise.
    """
    # Check PATH first
    try:
        subprocess.run(
            ["make", "--version"],
            capture_output=True,
            check=True,
        )
        return "make"
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    if platform.system().lower() == "windows":
        ndk_path = get_android_ndk_path()
        if ndk_path:
            prebuilt_dir = os.path.join(ndk_path, "prebuilt")
            if os.path.isdir(prebuilt_dir):
                for host_dir in os.listdir(prebuilt_dir):
                    make_path = os.path.join(prebuilt_dir, host_dir, "bin", "make.exe")
                    if os.path.exists(make_path):
                        return make_path

    return None


def get_cmake_generator(target: BuildTarget, vs_version: Optional[str] = None) -> str:
    """Get the appropriate CMake generator for the target.

    Args:
        target: The build target
        vs_version: Optional Visual Studio version year (e.g., "2022", "2026").
                   If not specified, auto-detects the latest installed version.
    """
    # Android and iOS cross-compilation always use Ninja or Makefiles,
    # even when building on a Windows host
    if target.os in (OS.ANDROID, OS.IOS):
        ninja = find_ninja_program()
        if ninja:
            return "Ninja"
        make = find_make_program()
        if make:
            return "Unix Makefiles"
        raise RuntimeError(
            "No suitable build tool found for Android/iOS cross-compilation.\n"
            "Neither Ninja nor Make were found on PATH or in the Android SDK/NDK.\n"
            "Install Ninja (recommended) or ensure the NDK's make.exe is accessible.\n"
            "  - Install CMake via Android Studio SDK Manager (includes Ninja)\n"
            "  - Or install Ninja: https://github.com/nicknisi/ninja-build/releases"
        )

    if target.os == OS.WINDOWS:
        if vs_version:
            # User specified a version - look up the generator string
            generator = _get_vs_generator_for_version(vs_version)
            if generator:
                return generator
            # Fall through to auto-detection if specified version not found
            print(f"Warning: Visual Studio {vs_version} not found, auto-detecting...")

        # Auto-detect installed Visual Studio version
        vs_generator = _detect_visual_studio_version()
        if vs_generator:
            return vs_generator
        # Fallback to VS 2022 if detection fails
        return "Visual Studio 17 2022"
    else:
        # Prefer Ninja if available
        try:
            subprocess.run(
                ["ninja", "--version"],
                capture_output=True,
                check=True,
            )
            return "Ninja"
        except (subprocess.CalledProcessError, FileNotFoundError):
            return "Unix Makefiles"


def _get_vs_generator_for_version(year: str) -> Optional[str]:
    """Get the CMake generator string for a specific Visual Studio version.

    Args:
        year: The Visual Studio year (e.g., "2022", "2026")

    Returns:
        CMake generator string if found, None otherwise
    """
    # Map year to version number
    year_to_version = {
        "2017": "15",
        "2019": "16",
        "2022": "17",
        "2026": "18",
    }

    version = year_to_version.get(year)
    if not version:
        # Try to detect from installed VS
        vs_info = _get_vs_info_for_year(year)
        if vs_info:
            return vs_info
        return None

    # Verify this version is actually installed
    vswhere_paths = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
        r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe",
    ]

    vswhere_path = None
    for path in vswhere_paths:
        if os.path.exists(path):
            vswhere_path = path
            break

    if not vswhere_path:
        # No vswhere, just return the generator string and let CMake validate
        return f"Visual Studio {version} {year}"

    try:
        # Check if this specific version is installed
        # Use -version to filter by major version range
        result = subprocess.run(
            [
                vswhere_path,
                "-version",
                f"[{version}.0,{int(version) + 1}.0)",
                "-property",
                "installationPath",
            ],
            capture_output=True,
            text=True,
        )
        if result.stdout.strip():
            return f"Visual Studio {version} {year}"

        # If version range query failed, try searching all installations for the year
        result = subprocess.run(
            [vswhere_path, "-all", "-format", "json"],
            capture_output=True,
            text=True,
        )
        if result.returncode == 0 and result.stdout.strip():
            import json

            try:
                installations = json.loads(result.stdout)
                for install in installations:
                    display_name = install.get("displayName", "")
                    if year in display_name:
                        return f"Visual Studio {version} {year}"
            except json.JSONDecodeError:
                pass

        # Last resort: if the user explicitly requested this version, trust them
        # and return the generator string - CMake will error if it's not installed
        return f"Visual Studio {version} {year}"

    except (subprocess.CalledProcessError, FileNotFoundError):
        # If vswhere fails, still return the generator string and let CMake validate
        return f"Visual Studio {version} {year}"


def _get_vs_info_for_year(year: str) -> Optional[str]:
    """Try to find VS installation info for a specific year."""
    vswhere_paths = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
        r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe",
    ]

    vswhere_path = None
    for path in vswhere_paths:
        if os.path.exists(path):
            vswhere_path = path
            break

    if not vswhere_path:
        return None

    try:
        # Get all installed versions
        result = subprocess.run(
            [vswhere_path, "-all", "-format", "json"],
            capture_output=True,
            text=True,
            check=True,
        )
        import json

        installations = json.loads(result.stdout)
        for install in installations:
            display_name = install.get("displayName", "")
            if year in display_name:
                # Found matching year
                version = install.get("catalog", {}).get("productLineVersion", "")
                if version:
                    return f"Visual Studio {version} {year}"
    except (subprocess.CalledProcessError, FileNotFoundError, json.JSONDecodeError):
        pass

    return None


def _detect_visual_studio_version() -> Optional[str]:
    """Detect the installed Visual Studio version for CMake generator.

    Returns the CMake generator string like "Visual Studio 17 2022" or None if not found.
    """
    # Use vswhere to find installed Visual Studio instances
    vswhere_paths = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
        r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe",
    ]

    vswhere_path = None
    for path in vswhere_paths:
        if os.path.exists(path):
            vswhere_path = path
            break

    if not vswhere_path:
        return None

    # Map year to CMake generator version number
    year_to_version = {
        "2017": "15",
        "2019": "16",
        "2022": "17",
        "2026": "18",
    }

    try:
        # Get the installation version (e.g., "17.x.y" for VS 2022)
        result = subprocess.run(
            [vswhere_path, "-latest", "-property", "installationVersion"],
            capture_output=True,
            text=True,
            check=True,
        )
        install_version = result.stdout.strip()
        major_version = install_version.split(".")[0] if install_version else ""

        # Get the display name to extract the year (e.g., "Visual Studio Professional 2022")
        result = subprocess.run(
            [vswhere_path, "-latest", "-property", "displayName"],
            capture_output=True,
            text=True,
            check=True,
        )
        display_name = result.stdout.strip()

        # Extract year from display name (last 4 digits)
        import re

        year_match = re.search(r"(\d{4})$", display_name)
        if year_match and major_version:
            year = year_match.group(1)
            return f"Visual Studio {major_version} {year}"

        # Fallback: if we got a year but no major version, use the mapping
        if year_match:
            year = year_match.group(1)
            version = year_to_version.get(year)
            if version:
                return f"Visual Studio {version} {year}"

    except (subprocess.CalledProcessError, FileNotFoundError, IndexError):
        pass

    return None


def get_installed_windows_archs() -> List[Arch]:
    """Detect which Windows architectures have MSVC tools installed via vswhere.

    Probes for 64-bit architecture components only (x86_64 and ARM64).
    Falls back to the host architecture if vswhere is unavailable or no
    components are detected.
    """
    vswhere_paths = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
        r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe",
    ]
    vswhere_path = None
    for path in vswhere_paths:
        if os.path.exists(path):
            vswhere_path = path
            break

    if not vswhere_path:
        return [detect_host_arch()]

    # Map VS component IDs to architectures (64-bit only)
    component_to_arch = {
        "Microsoft.VisualStudio.Component.VC.Tools.x86.x64": Arch.X86_64,
        "Microsoft.VisualStudio.Component.VC.Tools.ARM64": Arch.ARM64,
    }

    archs = []
    for component, arch in component_to_arch.items():
        try:
            result = subprocess.run(
                [
                    vswhere_path,
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    component,
                    "-property",
                    "installationPath",
                ],
                capture_output=True,
                text=True,
                check=True,
            )
            if result.stdout.strip():
                archs.append(arch)
        except subprocess.CalledProcessError:
            pass

    return archs if archs else [detect_host_arch()]


def parse_platform_string(platform_str: str) -> tuple[OS, Arch]:
    """Parse a platform string like 'ios_arm64' into (OS, Arch).

    Args:
        platform_str: Platform string in format 'os_arch' (e.g., 'ios_arm64', 'macos_x64', 'android_x86_64')

    Returns:
        Tuple of (OS, Arch)

    Raises:
        ValueError: If the string format is invalid or values are unknown
    """
    platform_lower = platform_str.lower()

    # Split only on first underscore to handle arch names with underscores (e.g., x86_64)
    parts = platform_lower.split("_", 1)

    if len(parts) != 2:
        raise ValueError(
            f"Invalid platform string: {platform_str}. "
            "Expected format: os_arch (e.g., ios_arm64, macos_x64, android_x86_64)"
        )

    try:
        os_val = OS(parts[0])
    except ValueError:
        valid_os = ", ".join(o.value for o in OS)
        raise ValueError(f"Unknown OS: {parts[0]}. Valid options: {valid_os}")

    try:
        arch_val = Arch(parts[1])
    except ValueError:
        valid_arch = ", ".join(a.value for a in Arch)
        raise ValueError(
            f"Unknown architecture: {parts[1]}. Valid options: {valid_arch}"
        )

    return os_val, arch_val


def parse_target_string(target_str: str) -> BuildTarget:
    """Parse a target string like 'macos_arm64_static_debug' into BuildTarget.

    Formats:
        Non-Windows: os_arch_linktype[_debug]
        Windows: os_arch_toolset_linktype[_debug]

    Examples:
        macos_arm64_static
        macos_arm64_static_debug
        windows_x86_64_vc143_static
        windows_x86_64_vc143_static_debug

    Note: This function is deprecated. Prefer using parse_platform_string()
    and combining with explicit config/link parameters.
    """
    parts = target_str.lower().split("_")

    if len(parts) < 3:
        raise ValueError(
            f"Invalid target string: {target_str}. "
            "Expected format: os_arch_linktype[_debug] or os_arch_toolset_linktype[_debug]"
        )

    try:
        os_val = OS(parts[0])
    except ValueError:
        raise ValueError(f"Unknown OS: {parts[0]}")

    try:
        arch_val = Arch(parts[1])
    except ValueError:
        raise ValueError(f"Unknown architecture: {parts[1]}")

    # Windows targets include MSVC toolset version (e.g., vc143)
    msvc_toolset = None
    link_idx = 2
    if os_val == OS.WINDOWS:
        if len(parts) < 4:
            raise ValueError(
                f"Invalid Windows target string: {target_str}. "
                "Expected format: windows_arch_toolset_linktype[_debug]"
            )
        # Check if parts[2] is a toolset version (starts with 'vc')
        if parts[2].startswith("vc"):
            msvc_toolset = parts[2]
            link_idx = 3
        else:
            raise ValueError(
                f"Invalid Windows target string: {target_str}. "
                "Expected toolset version like 'vc143' after architecture"
            )

    try:
        link_val = LinkType(parts[link_idx])
    except ValueError:
        raise ValueError(f"Unknown link type: {parts[link_idx]}")

    build_config = BuildConfig.RELEASE
    if len(parts) > link_idx + 1 and parts[link_idx + 1] == "debug":
        build_config = BuildConfig.DEBUG

    return BuildTarget(
        os=os_val,
        arch=arch_val,
        build_config=build_config,
        link_type=link_val,
        msvc_toolset=msvc_toolset,
    )
