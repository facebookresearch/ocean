# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Platform detection and build target definitions."""

from __future__ import annotations

import json
import os
import platform
import re
import subprocess
import sys
from dataclasses import dataclass
from enum import Enum
from typing import Dict, List, Optional


def configure_console_encoding() -> None:
    """Force UTF-8 on stdout/stderr so the status glyphs survive redirection.

    On Windows a console-attached stdout uses UTF-8, but a pipe or a redirected
    file falls back to the ANSI code page. cp1252 cannot encode the U+2713 /
    U+2717 / U+2550 characters the build prints, so `build_ocean_3rdparty.py >
    build.log` aborts on the very first status line with a UnicodeEncodeError
    that looks nothing like its cause.
    """
    for stream in (sys.stdout, sys.stderr):
        reconfigure = getattr(stream, "reconfigure", None)
        if reconfigure is None:
            continue
        try:
            reconfigure(encoding="utf-8", errors="replace")
        except (OSError, ValueError):
            pass


# Minimum Android version the third-party libraries target by default.
# Overridable with --android-api-level; kept here so the builder and the
# metadata that records it cannot disagree.
DEFAULT_ANDROID_API_LEVEL = 32


class OS(Enum):
    """Target operating system."""

    MACOS = "macos"
    IOS = "ios"
    LINUX = "linux"
    ANDROID = "android"
    WINDOWS = "win"


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


# Visual Studio component IDs providing the 64-bit MSVC cross-tools, by target
# architecture. These IDs are stable across Visual Studio versions, unlike the
# display names, which are localized and carry the toolset number.
WINDOWS_ARCH_COMPONENT_IDS: Dict[Arch, str] = {
    Arch.X86_64: "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
    Arch.ARM64: "Microsoft.VisualStudio.Component.VC.Tools.ARM64",
}

# Human-readable form of the above, for error messages.
WINDOWS_ARCH_COMPONENTS: Dict[Arch, str] = {
    Arch.X86_64: (
        "MSVC x64/x86 build tools (Microsoft.VisualStudio.Component.VC.Tools.x86.x64)"
    ),
    Arch.ARM64: (
        "MSVC ARM64/ARM64EC build tools "
        "(Microsoft.VisualStudio.Component.VC.Tools.ARM64)"
    ),
}


# Visual Studio year <-> the major version number that appears in CMake's
# generator name ("Visual Studio 17 2022") and in vswhere's installationVersion.
_VS_YEAR_TO_MAJOR: Dict[str, str] = {
    "2017": "15",
    "2019": "16",
    "2022": "17",
    "2026": "18",
}
_VS_MAJOR_TO_YEAR: Dict[str, str] = {v: k for k, v in _VS_YEAR_TO_MAJOR.items()}
# The MSVC toolset is versioned independently of the product: Visual Studio 17
# (2022) ships compiler 14.3, which Ocean writes as "vc143". The two numbers
# must never be derived from one another by string surgery.
_VS_MAJOR_TO_TOOLSET: Dict[str, str] = {
    "15": "vc141",
    "16": "vc142",
    "17": "vc143",
    "18": "vc145",
}
_VS_TOOLSET_TO_MAJOR: Dict[str, str] = {v: k for k, v in _VS_MAJOR_TO_TOOLSET.items()}


def find_vswhere() -> Optional[str]:
    """Locate vswhere.exe, the Visual Studio installation query tool.

    The installer roots come from the environment rather than being hard-coded
    to C:, so this agrees with preflight's own probe on hosts where Program
    Files is not on the system drive.
    """
    roots = (
        os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
        os.environ.get("ProgramFiles", r"C:\Program Files"),
    )
    for root in roots:
        path = os.path.join(root, "Microsoft Visual Studio", "Installer", "vswhere.exe")
        if os.path.exists(path):
            return path
    return None


def get_msvc_toolset_version(vs_version: Optional[str] = None) -> Optional[str]:
    """Get the MSVC toolset version.

    Args:
        vs_version: Optional Visual Studio version year (e.g., "2022", "2026").
                   If specified, returns the toolset for that version.
                   If not specified, auto-detects the latest installed version.

    Returns toolset version string like 'vc143' (VS2022), 'vc145' (VS2026), or 'vc142' (VS2019).
    Returns None if not on Windows or MSVC is not available.
    """
    toolset, _ = get_msvc_toolset_version_and_path(vs_version)
    return toolset


def get_all_installed_vs_versions() -> list[tuple[str, str, str]]:
    """Detect all installed Visual Studio versions using vswhere.

    Returns:
        List of tuples (year, toolset, path) for each installed VS version,
        sorted from newest to oldest. Returns empty list if not on Windows
        or vswhere is not available.
    """
    if platform.system().lower() != "windows":
        return []

    vswhere_path = find_vswhere()
    if not vswhere_path:
        return []

    try:
        # Get all VS installations in JSON format. '-products *' includes the
        # standalone Build Tools SKU, which is what CI hosts typically have;
        # these flags must stay in sync with the component query in
        # get_installed_windows_archs().
        result = subprocess.run(
            [vswhere_path, "-all", "-products", "*", "-format", "json"],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=True,
        )

        if not result.stdout.strip():
            return []

        installations = json.loads(result.stdout)
        installed = []

        for install in installations:
            install_path = install.get("installationPath", "")
            install_version = install.get("installationVersion", "")
            display_name = install.get("displayName", "")

            if not install_path or not install_version:
                continue

            # Extract major version (e.g., "17" from "17.8.34525.116")
            major_version = install_version.split(".")[0] if install_version else ""

            # installationVersion is the authoritative, stable identifier, so
            # it decides the year wherever we know the product major.
            # displayName is localized marketing text, is empty for some Build
            # Tools installs, and can carry a trailing number of its own
            # ("Visual Studio Community LTSC 2024" on a 2022 install), so it is
            # only consulted for a major we do not recognise.
            year = _VS_MAJOR_TO_YEAR.get(major_version)
            if not year:
                # Note: not f"20{major}" — that yields "2017" for VS 2022
                # (major 17), which then sorts the newest install to the bottom.
                year_match = re.search(r"(\d{4})$", display_name)
                year = year_match.group(1) if year_match else major_version

            toolset = _VS_MAJOR_TO_TOOLSET.get(major_version, f"vc{major_version}")

            # Append \VC to get the VC directory path
            vc_path = os.path.join(install_path, "VC")

            installed.append((year, toolset, vc_path, major_version))

        # Sort by major version descending (newest first). Sorting on the year
        # string would misorder an entry whose year had to be derived.
        installed.sort(key=lambda x: int(x[3]) if x[3].isdigit() else -1, reverse=True)
        return [(year, toolset, path) for year, toolset, path, _major in installed]

    except (subprocess.SubprocessError, OSError, json.JSONDecodeError):
        return []


def get_msvc_toolset_version_and_path(
    vs_version: Optional[str] = None,
) -> tuple[Optional[str], Optional[str]]:
    """Get the MSVC toolset version and installation path.

    Uses vswhere.exe to detect installed Visual Studio versions.

    Args:
        vs_version: Optional Visual Studio version year (e.g., "2022", "2026").
                   If specified, returns the toolset for that version.
                   If not specified, auto-detects the latest installed version.

    Returns:
        Tuple of (toolset_version, install_path).
        toolset_version: String like 'vc143' (VS2022), 'vc145' (VS2026), or 'vc142' (VS2019).
        install_path: Path to the Visual Studio VC directory.
        Returns (None, None) if not on Windows or MSVC is not available.
    """
    if platform.system().lower() != "windows":
        return None, None

    # Get all installed VS versions via vswhere
    installed = get_all_installed_vs_versions()

    if not installed:
        # Fallback to cl.exe detection if vswhere fails
        return _detect_toolset_from_cl()

    # If a specific version is requested, find it
    if vs_version:
        for year, toolset, path in installed:
            if year == vs_version:
                return toolset, path
        # Version not found, return None
        return None, None

    # Return the newest installed version (list is sorted newest first)
    year, toolset, path = installed[0]
    return toolset, path


def _detect_toolset_from_cl() -> tuple[Optional[str], Optional[str]]:
    """Fallback detection using cl.exe version output.

    Returns:
        Tuple of (toolset_version, None) since we can't determine path from cl.exe.
    """
    try:
        result = subprocess.run(
            ["cl"],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
        )
        # cl.exe outputs version info to stderr
        version_output = result.stderr
        if "19.5" in version_output:  # VS2026 18.x (estimated)
            return "vc145", None
        elif "19.4" in version_output:  # VS2022 17.x
            return "vc143", None
        elif "19.3" in version_output:  # VS2022 17.0-17.3
            return "vc143", None
        elif "19.2" in version_output:  # VS2019
            return "vc142", None
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    # Default to vc143 (VS2022) if we can't detect
    return "vc143", None


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
            win_x64_vc143_static
            win_x64_vc143_static_debug
        """
        arch_str = self.arch.value
        if self.os == OS.WINDOWS and self.arch == Arch.X86_64:
            arch_str = "x64"
        parts = [self.os.value, arch_str]
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


def _is_ndk_root(path: str) -> bool:
    """An NDK root is identified by the CMake toolchain file we hand to CMake."""
    return os.path.isfile(
        os.path.join(path, "build", "cmake", "android.toolchain.cmake")
    )


def _ndk_version_key(name: str) -> tuple:
    """Sort key for NDK directory names such as '27.0.12077973' or 'r21e'.

    Compared numerically per component: a lexicographic sort puts '9.x' above
    '27.x', which would pick a years-old NDK on a host that has both.
    """
    parts = re.findall(r"\d+", name)
    return tuple(int(p) for p in parts) if parts else (0,)


def get_android_ndk_path() -> Optional[str]:
    """Get Android NDK path from environment or common locations.

    Every candidate is validated by looking for android.toolchain.cmake, so a
    stale environment variable or an unrelated directory does not produce a
    path the build only rejects much later. When several NDKs are installed
    side by side the newest wins, compared numerically.

    Checks the following environment variables (in order):
    - ANDROID_NDK_HOME (official recommended name)
    - ANDROID_NDK (common shorthand)
    - NDK_HOME (alternative)
    - ANDROID_NDK_ROOT (used in some documentation)
    - NDK_ROOT (used in some build systems)

    Then SDK roots (ANDROID_HOME, ANDROID_SDK_ROOT and the per-platform
    defaults), looking at both the modern ndk/<version>/ layout and the legacy
    ndk-bundle/ one, and finally /opt/android-ndk.
    """
    for var in (
        "ANDROID_NDK_HOME",
        "ANDROID_NDK",
        "NDK_HOME",
        "ANDROID_NDK_ROOT",
        "NDK_ROOT",
    ):
        ndk_path = os.environ.get(var)
        if ndk_path and _is_ndk_root(ndk_path):
            return ndk_path

    sdk_roots = [
        os.environ.get("ANDROID_HOME"),
        os.environ.get("ANDROID_SDK_ROOT"),
        os.path.expanduser("~/Library/Android/sdk"),  # macOS
        os.path.expanduser("~/Android/Sdk"),  # Linux
        os.path.expanduser("~/AppData/Local/Android/Sdk"),  # Windows
    ]

    candidates = []
    for sdk_root in sdk_roots:
        if sdk_root:
            candidates.append(os.path.join(sdk_root, "ndk"))
            candidates.append(os.path.join(sdk_root, "ndk-bundle"))
    candidates.append("/opt/android-ndk")

    for base in candidates:
        # ndk-bundle (and a directly-pointed-at NDK) is itself the root.
        if _is_ndk_root(base):
            return base
        if not os.path.isdir(base):
            continue
        try:
            versions = [
                name
                for name in os.listdir(base)
                if _is_ndk_root(os.path.join(base, name))
            ]
        except OSError:
            continue
        if versions:
            return os.path.join(base, max(versions, key=_ndk_version_key))

    return None


def get_ios_sdk_path(sdk_type: str = "iphoneos") -> Optional[str]:
    """Get iOS SDK path using xcrun."""
    try:
        result = subprocess.run(
            ["xcrun", "--sdk", sdk_type, "--show-sdk-path"],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
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


def _vs_major_version(year: str, toolset: str) -> Optional[str]:
    """The number CMake wants in "Visual Studio <major> <year>".

    Three numbering schemes are in play and they are not interchangeable: the
    year (2022), the Visual Studio product major version (17, as reported by
    vswhere's installationVersion), and the MSVC toolset version (compiler
    14.3, which Ocean writes as "vc143"). Only the product major belongs in a
    generator name — reading it off "vc143" gives 14, which is Visual Studio
    2015.
    """
    major = _VS_YEAR_TO_MAJOR.get(year) or _VS_TOOLSET_TO_MAJOR.get(toolset)
    if major:
        return major
    # get_all_installed_vs_versions() synthesises f"vc{major}" for a product
    # major it does not recognise, so a future release still round-trips.
    suffix = toolset.removeprefix("vc")
    return suffix if suffix.isdigit() and len(suffix) <= 2 else None


def _generator_name(year: str, major_version: Optional[str] = None) -> Optional[str]:
    """CMake generator string for a Visual Studio year, e.g. 'Visual Studio 17 2022'."""
    major = major_version or _VS_YEAR_TO_MAJOR.get(year)
    return f"Visual Studio {major} {year}" if major else None


def _get_vs_generator_for_version(year: str) -> Optional[str]:
    """Get the CMake generator string for a specific Visual Studio version.

    Args:
        year: The Visual Studio year (e.g., "2022", "2026")

    Returns:
        CMake generator string if the year is installed or known, None otherwise
    """
    for installed_year, toolset, _path in get_all_installed_vs_versions():
        if installed_year == year:
            return _generator_name(year, _vs_major_version(year, toolset))

    # Not installed, or vswhere is unavailable. For a year we know the mapping
    # for, trust the user and let CMake produce the (clear) error if it is
    # wrong; --vs-version is validated separately against the installed set.
    return _generator_name(year)


def _detect_visual_studio_version() -> Optional[str]:
    """Detect the newest installed Visual Studio, as a CMake generator string.

    Delegates to get_all_installed_vs_versions() rather than running its own
    vswhere queries. The previous `-latest` probes here omitted `-products *`,
    which excludes the standalone Build Tools SKU that CI hosts use: on such a
    host detection returned None and get_cmake_generator() fell through to a
    hard-coded "Visual Studio 17 2022", so every library failed to configure
    with "could not find any instance of Visual Studio" — after all sources had
    already been cloned.
    """
    installed = get_all_installed_vs_versions()
    if not installed:
        return None
    year, toolset, _path = installed[0]
    return _generator_name(year, _vs_major_version(year, toolset))


def _normalize_install_path(path: str) -> str:
    """Normalize a Visual Studio path for case-insensitive comparison."""
    return os.path.normcase(os.path.normpath(path.strip()))


def _vs_install_root(vc_path: str) -> str:
    """Convert a '<install>\\VC' path back to the installation root.

    get_all_installed_vs_versions() appends 'VC' because that is what CMake and
    the toolchain files expect, but vswhere reports and matches on the root.
    """
    if os.path.basename(vc_path).lower() == "vc":
        return os.path.dirname(vc_path)
    return vc_path


def detect_windows_archs(
    vs_version: Optional[str] = None,
) -> tuple[List[Arch], bool]:
    """Detect which Windows architectures have MSVC tools installed.

    Probes for 64-bit architecture components only (x86_64 and ARM64).

    The probe is scoped to the Visual Studio installation that the build will
    actually use. With several versions installed it is common for one to carry
    the ARM64 tools and another not; an unscoped query reports ARM64 as
    available and the build then fails deep inside MSBuild with an opaque
    'BaseOutputPath/OutputPath is not set for VCTargetsPath.vcxproj'.

    Args:
        vs_version: Visual Studio year to scope to (e.g. "2022"). Defaults to
            the auto-selected (newest) installation.

    Returns:
        (archs, scoped). When `scoped` is True the answer is authoritative:
        an architecture missing from `archs` genuinely has no tools in the
        selected installation. When it is False no installation could be
        identified at all (no vswhere, or nothing detectable), `archs` is a
        host-architecture guess, and callers must neither claim an
        architecture is "not installed in the selected Visual Studio" nor
        refuse to build it.
    """
    vswhere_path = find_vswhere()
    if not vswhere_path:
        return [detect_host_arch()], False

    _, selected_vc_path = get_msvc_toolset_version_and_path(vs_version)
    selected_root = (
        _normalize_install_path(_vs_install_root(selected_vc_path))
        if selected_vc_path
        else None
    )

    if selected_root is None:
        # No installation to scope to (vswhere found nothing, the requested
        # --vs-version is absent, or detection fell back to bare cl.exe).
        # Scoping is impossible, so report the host architecture rather than
        # silently reverting to "any installation has these tools".
        return [detect_host_arch()], False

    archs = []
    for arch, component in WINDOWS_ARCH_COMPONENT_IDS.items():
        try:
            result = subprocess.run(
                # These flags must stay identical to the query in
                # get_all_installed_vs_versions(): selected_root comes from
                # there, and a narrower filter here would drop the very
                # installation being matched against.
                [
                    vswhere_path,
                    "-all",
                    "-products",
                    "*",
                    "-requires",
                    component,
                    "-property",
                    "installationPath",
                ],
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                check=True,
            )
        # OSError covers vswhere becoming inaccessible between find_vswhere()
        # and here; SubprocessError covers a non-zero exit and any future
        # timeout. Neither should abort platform detection for every
        # architecture — skip this one and carry on.
        except (subprocess.SubprocessError, OSError):
            continue

        roots = {
            _normalize_install_path(line)
            for line in result.stdout.splitlines()
            if line.strip()
        }
        if selected_root in roots:
            archs.append(arch)

    return archs, True


def get_installed_windows_archs(vs_version: Optional[str] = None) -> List[Arch]:
    """Architectures the selected Visual Studio can build for.

    Convenience wrapper around detect_windows_archs() for callers that do not
    need to know whether the probe could be scoped.
    """
    return detect_windows_archs(vs_version)[0]


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
        # Accept "windows" as a backward-compatible alias for "win"
        if parts[0] == "windows":
            os_val = OS.WINDOWS
        else:
            valid_os = ", ".join(o.value for o in OS)
            raise ValueError(f"Unknown OS: {parts[0]}. Valid options: {valid_os}")

    arch_str = parts[1]
    # Accept "x64" as an alias for "x86_64" (used in Windows target paths)
    if arch_str == "x64":
        arch_str = "x86_64"
    try:
        arch_val = Arch(arch_str)
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
        win_x64_vc143_static
        win_x64_vc143_static_debug

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
        # Accept "windows" as a backward-compatible alias for "win"
        if parts[0] == "windows":
            os_val = OS.WINDOWS
        else:
            raise ValueError(f"Unknown OS: {parts[0]}")

    # Accept "x64" as an alias for "x86_64" (used in Windows target paths)
    arch_str = parts[1]
    if arch_str == "x64":
        arch_str = "x86_64"
    try:
        arch_val = Arch(arch_str)
    except ValueError:
        raise ValueError(f"Unknown architecture: {parts[1]}")

    # Windows targets include MSVC toolset version (e.g., vc143)
    msvc_toolset = None
    link_idx = 2
    if os_val == OS.WINDOWS:
        if len(parts) < 4:
            raise ValueError(
                f"Invalid Windows target string: {target_str}. "
                "Expected format: win_arch_toolset_linktype[_debug]"
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
