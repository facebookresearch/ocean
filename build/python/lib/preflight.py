# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

# @nolint

"""Pre-flight dependency checks for the build system."""

from __future__ import annotations

import os
import shutil
import subprocess
import sys  # noqa: F401
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Tuple


class LogLevel(Enum):
    """Log level for build output."""

    ERROR = "error"
    WARNING = "warning"
    STATUS = "status"
    VERBOSE = "verbose"
    DEBUG = "debug"

    @classmethod
    def from_string(cls, value: str) -> "LogLevel":
        """Parse log level from string."""
        try:
            return cls(value.lower())
        except ValueError:
            valid = ", ".join(level.value for level in cls)
            raise ValueError(f"Unknown log level: {value}. Valid options: {valid}")

    def __ge__(self, other: "LogLevel") -> bool:
        """Compare log levels (ERROR < WARNING < STATUS < VERBOSE < DEBUG)."""
        order = [
            LogLevel.ERROR,
            LogLevel.WARNING,
            LogLevel.STATUS,
            LogLevel.VERBOSE,
            LogLevel.DEBUG,
        ]
        return order.index(self) >= order.index(other)

    def __gt__(self, other: "LogLevel") -> bool:
        order = [
            LogLevel.ERROR,
            LogLevel.WARNING,
            LogLevel.STATUS,
            LogLevel.VERBOSE,
            LogLevel.DEBUG,
        ]
        return order.index(self) > order.index(other)


# Minimum required versions
MIN_CMAKE_VERSION = (
    3,
    25,
)  # CMake 3.25 required for modern Android toolchain and preset support
MIN_GIT_VERSION = (2, 0)


@dataclass
class VersionInfo:
    """Version information for a tool."""

    name: str
    path: Optional[str]
    version: Optional[Tuple[int, ...]]
    version_string: Optional[str]
    required_version: Tuple[int, ...]
    is_available: bool
    meets_requirement: bool


def parse_version(version_str: str) -> Tuple[int, ...]:
    """Parse a version string like '3.28.1' into a tuple of ints."""
    # Extract just the version numbers, handling formats like:
    # "cmake version 3.28.1"
    # "git version 2.39.3 (Apple Git-146)"
    parts = version_str.split()
    for part in parts:
        if part[0].isdigit():
            # Remove any non-numeric suffix
            version_part = part.split("-")[0].split("+")[0]
            try:
                return tuple(int(x) for x in version_part.split(".") if x.isdigit())
            except ValueError:
                continue
    return ()


def _which_with_fallback(tool: str, fallback_paths: List[Path]) -> Optional[str]:
    """Find a tool on PATH, falling back to common installation directories.

    On Windows, tools like CMake may not be in PATH (especially inside a venv),
    so we check well-known installation directories as a fallback. When found
    via fallback, the tool's directory is added to PATH so subsequent subprocess
    calls can find it.
    """
    path = shutil.which(tool)
    if path:
        return path

    if os.name == "nt":
        for candidate in fallback_paths:
            if candidate.exists():
                # Add to PATH so the rest of the build can find it
                tool_dir = str(candidate.parent)
                os.environ["PATH"] = tool_dir + os.pathsep + os.environ.get("PATH", "")
                return str(candidate)

    return None


# Common Windows installation paths for tools
_CMAKE_WINDOWS_PATHS = [
    Path(os.environ.get("ProgramFiles", "C:\\Program Files")) / "CMake" / "bin" / "cmake.exe",
    Path(os.environ.get("ProgramFiles(x86)", "C:\\Program Files (x86)")) / "CMake" / "bin" / "cmake.exe",
    Path(os.environ.get("LOCALAPPDATA", "")) / "CMake" / "bin" / "cmake.exe",
]


def check_cmake() -> VersionInfo:
    """Check if CMake is installed and meets version requirements."""
    path = _which_with_fallback("cmake", _CMAKE_WINDOWS_PATHS)
    if not path:
        return VersionInfo(
            name="cmake",
            path=None,
            version=None,
            version_string=None,
            required_version=MIN_CMAKE_VERSION,
            is_available=False,
            meets_requirement=False,
        )

    try:
        result = subprocess.run(
            [path, "--version"],
            capture_output=True,
            text=True,
            check=True,
        )
        first_line = result.stdout.split("\n")[0]
        version = parse_version(first_line)
        meets_req = version >= MIN_CMAKE_VERSION if version else False

        return VersionInfo(
            name="cmake",
            path=path,
            version=version,
            version_string=first_line,
            required_version=MIN_CMAKE_VERSION,
            is_available=True,
            meets_requirement=meets_req,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return VersionInfo(
            name="cmake",
            path=path,
            version=None,
            version_string=None,
            required_version=MIN_CMAKE_VERSION,
            is_available=False,
            meets_requirement=False,
        )


def check_git() -> VersionInfo:
    """Check if Git is installed and meets version requirements."""
    path = shutil.which("git")
    if not path:
        return VersionInfo(
            name="git",
            path=None,
            version=None,
            version_string=None,
            required_version=MIN_GIT_VERSION,
            is_available=False,
            meets_requirement=False,
        )

    try:
        result = subprocess.run(
            ["git", "--version"],
            capture_output=True,
            text=True,
            check=True,
        )
        first_line = result.stdout.strip()
        version = parse_version(first_line)
        meets_req = version >= MIN_GIT_VERSION if version else False

        return VersionInfo(
            name="git",
            path=path,
            version=version,
            version_string=first_line,
            required_version=MIN_GIT_VERSION,
            is_available=True,
            meets_requirement=meets_req,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return VersionInfo(
            name="git",
            path=path,
            version=None,
            version_string=None,
            required_version=MIN_GIT_VERSION,
            is_available=False,
            meets_requirement=False,
        )


def check_ninja() -> VersionInfo:
    """Check if Ninja is available (optional, for faster builds)."""
    path = shutil.which("ninja")
    if not path:
        return VersionInfo(
            name="ninja",
            path=None,
            version=None,
            version_string=None,
            required_version=(),
            is_available=False,
            meets_requirement=True,  # Ninja is optional
        )

    try:
        result = subprocess.run(
            ["ninja", "--version"],
            capture_output=True,
            text=True,
            check=True,
        )
        version_str = result.stdout.strip()
        version = parse_version(version_str)

        return VersionInfo(
            name="ninja",
            path=path,
            version=version,
            version_string=f"ninja {version_str}",
            required_version=(),
            is_available=True,
            meets_requirement=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return VersionInfo(
            name="ninja",
            path=None,
            version=None,
            version_string=None,
            required_version=(),
            is_available=False,
            meets_requirement=True,
        )


def format_version(version: Tuple[int, ...]) -> str:
    """Format a version tuple as a string."""
    return ".".join(str(x) for x in version)


def run_preflight_checks(log_level: LogLevel = LogLevel.STATUS) -> bool:
    """Run all pre-flight checks and report results.

    Returns True if all required checks pass, False otherwise.
    """
    print("Running pre-flight checks...")
    print()

    all_passed = True
    checks = [
        ("cmake", check_cmake(), True),  # Required
        ("git", check_git(), True),  # Required
        ("ninja", check_ninja(), False),  # Optional
    ]

    for name, info, required in checks:
        if not info.is_available:
            if required:
                print(f"  ✗ {name}: NOT FOUND")
                _print_install_instructions(name)
                all_passed = False
            elif log_level >= LogLevel.STATUS:
                print(f"  - {name}: not found (optional)")
        elif not info.meets_requirement:
            print(f"  ✗ {name}: {info.version_string}")
            print(
                f"      Requires version {format_version(info.required_version)} or higher"
            )
            all_passed = False
        else:
            if log_level >= LogLevel.STATUS:
                print(f"  ✓ {name}: {info.version_string}")

    print()

    if not all_passed:
        print("Pre-flight checks failed. Please install missing dependencies.")
        return False

    # On Windows, warn about Long Path support
    if os.name == "nt":
        _check_long_path_support(log_level)

    if log_level >= LogLevel.STATUS:
        print("Pre-flight checks passed.")
    return True


def _check_long_path_support(log_level: LogLevel) -> None:
    """Check if Windows Long Path support is enabled and warn if not.

    Build paths can easily exceed the 260-character MAX_PATH limit on Windows.
    This checks the registry to see if long paths are enabled and prints a
    warning with remediation steps if they are not.
    """
    try:
        import winreg

        key = winreg.OpenKey(
            winreg.HKEY_LOCAL_MACHINE,
            r"SYSTEM\CurrentControlSet\Control\FileSystem",
        )
        value, _ = winreg.QueryValueEx(key, "LongPathsEnabled")
        winreg.CloseKey(key)

        if value == 1:
            if log_level >= LogLevel.STATUS:
                print("  ✓ Windows Long Path support is enabled.")
            return
    except Exception:
        pass

    # Long paths not enabled or we couldn't check
    print("  ⚠ WARNING: Windows Long Path support is not enabled.")
    print("    Build paths may exceed the 260-character MAX_PATH limit, causing failures.")
    print("    To enable (requires Administrator privileges):")
    print("      reg add HKLM\\SYSTEM\\CurrentControlSet\\Control\\FileSystem /v LongPathsEnabled /t REG_DWORD /d 1 /f")
    print("    Or via Group Policy Editor:")
    print("      Computer Configuration > Administrative Templates > System > Filesystem")
    print("      > Enable Win32 long paths")
    print("    A system restart may be required after enabling this setting.")
    print()


def _print_install_instructions(tool: str) -> None:
    """Print installation instructions for a missing tool."""
    import platform

    system = platform.system().lower()

    instructions = {
        "cmake": {
            "darwin": [
                "    brew install cmake",
                "    # Or download from: https://cmake.org/download/",
            ],
            "linux": [
                "    # Ubuntu/Debian:",
                "    sudo apt-get install cmake",
                "    # Fedora/RHEL:",
                "    sudo dnf install cmake",
                "    # Or download from: https://cmake.org/download/",
            ],
            "windows": [
                "    # Using winget:",
                "    winget install Kitware.CMake",
                "    # Or download from: https://cmake.org/download/",
            ],
        },
        "git": {
            "darwin": [
                "    brew install git",
                "    # Or: xcode-select --install",
            ],
            "linux": [
                "    # Ubuntu/Debian:",
                "    sudo apt-get install git",
                "    # Fedora/RHEL:",
                "    sudo dnf install git",
            ],
            "windows": [
                "    # Download from: https://git-scm.com/download/win",
            ],
        },
        "ninja": {
            "darwin": [
                "    brew install ninja",
            ],
            "linux": [
                "    # Ubuntu/Debian:",
                "    sudo apt-get install ninja-build",
                "    # Fedora/RHEL:",
                "    sudo dnf install ninja-build",
            ],
            "windows": [
                "    # Using winget:",
                "    winget install Ninja-build.Ninja",
            ],
        },
        "xcode": {
            "darwin": [
                "    xcode-select --install",
                "    # Or install Xcode from the App Store",
            ],
        },
        "ios_sdk": {
            "darwin": [
                "    # If Xcode is installed but iOS platform is missing:",
                "    xcodebuild -downloadPlatform iOS",
                "    # Or install Xcode from the App Store (includes iOS SDK)",
            ],
        },
        "ios_simulator_sdk": {
            "darwin": [
                "    # If Xcode is installed but iOS Simulator is missing:",
                "    xcodebuild -downloadPlatform iOS",
                "    # Or install Xcode from the App Store",
            ],
        },
        "android_ndk": {
            "darwin": [
                "    # Set ANDROID_NDK_HOME environment variable",
                "    # Download NDK from: https://developer.android.com/ndk/downloads",
            ],
            "linux": [
                "    # Set ANDROID_NDK_HOME environment variable",
                "    # Download NDK from: https://developer.android.com/ndk/downloads",
            ],
            "windows": [
                "    # Set ANDROID_NDK_HOME environment variable",
                "    # Download NDK from: https://developer.android.com/ndk/downloads",
            ],
        },
        "visual_studio": {
            "windows": [
                "    # Download Visual Studio 2022 from:",
                "    # https://visualstudio.microsoft.com/downloads/",
                "    # During installation, select 'Desktop development with C++' workload",
            ],
        },
        "windows_sdk": {
            "windows": [
                "    # Install via Visual Studio Installer:",
                "    # 1. Open Visual Studio Installer",
                "    # 2. Click 'Modify' on your Visual Studio installation",
                "    # 3. Under 'Individual components', select 'Windows 10 SDK' or 'Windows 11 SDK'",
            ],
        },
    }

    if tool in instructions:
        platform_instructions = instructions[tool].get(system, [])
        if platform_instructions:
            print("    To install:")
            for line in platform_instructions:
                print(line)


# ============================================================================
# Toolchain Checks
# ============================================================================


@dataclass
class ToolchainInfo:
    """Information about a platform toolchain."""

    name: str
    is_available: bool
    path: Optional[str] = None
    version: Optional[str] = None
    error: Optional[str] = None


def check_xcode() -> ToolchainInfo:
    """Check if Xcode command line tools are installed."""
    try:
        result = subprocess.run(
            ["xcode-select", "-p"],
            capture_output=True,
            text=True,
            check=True,
        )
        xcode_path = result.stdout.strip()

        # Get Xcode version if full Xcode is installed
        version = None
        try:
            ver_result = subprocess.run(
                ["xcodebuild", "-version"],
                capture_output=True,
                text=True,
                check=True,
            )
            # Parse "Xcode 15.2" from first line
            first_line = ver_result.stdout.split("\n")[0]
            version = first_line
        except (subprocess.CalledProcessError, FileNotFoundError):
            # Only command line tools installed, not full Xcode
            version = "Command Line Tools"

        return ToolchainInfo(
            name="xcode",
            is_available=True,
            path=xcode_path,
            version=version,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ToolchainInfo(
            name="xcode",
            is_available=False,
            error="Xcode command line tools not installed",
        )


def check_macos_sdk() -> ToolchainInfo:
    """Check if macOS SDK is available."""
    try:
        result = subprocess.run(
            ["xcrun", "--sdk", "macosx", "--show-sdk-path"],
            capture_output=True,
            text=True,
            check=True,
        )
        sdk_path = result.stdout.strip()

        # Get SDK version
        version = None
        try:
            ver_result = subprocess.run(
                ["xcrun", "--sdk", "macosx", "--show-sdk-version"],
                capture_output=True,
                text=True,
                check=True,
            )
            version = ver_result.stdout.strip()
        except subprocess.CalledProcessError:
            pass

        return ToolchainInfo(
            name="macos_sdk",
            is_available=True,
            path=sdk_path,
            version=version,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ToolchainInfo(
            name="macos_sdk",
            is_available=False,
            error="macOS SDK not found",
        )


def check_ios_sdk() -> ToolchainInfo:
    """Check if iOS SDK is available."""
    try:
        result = subprocess.run(
            ["xcrun", "--sdk", "iphoneos", "--show-sdk-path"],
            capture_output=True,
            text=True,
            check=True,
        )
        sdk_path = result.stdout.strip()

        # Get SDK version
        version = None
        try:
            ver_result = subprocess.run(
                ["xcrun", "--sdk", "iphoneos", "--show-sdk-version"],
                capture_output=True,
                text=True,
                check=True,
            )
            version = ver_result.stdout.strip()
        except subprocess.CalledProcessError:
            pass

        return ToolchainInfo(
            name="ios_sdk",
            is_available=True,
            path=sdk_path,
            version=version,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ToolchainInfo(
            name="ios_sdk",
            is_available=False,
            error="iOS SDK not found (requires Xcode with iOS support)",
        )


def check_ios_simulator_sdk() -> ToolchainInfo:
    """Check if iOS Simulator SDK is available."""
    try:
        result = subprocess.run(
            ["xcrun", "--sdk", "iphonesimulator", "--show-sdk-path"],
            capture_output=True,
            text=True,
            check=True,
        )
        sdk_path = result.stdout.strip()

        # Get SDK version
        version = None
        try:
            ver_result = subprocess.run(
                ["xcrun", "--sdk", "iphonesimulator", "--show-sdk-version"],
                capture_output=True,
                text=True,
                check=True,
            )
            version = ver_result.stdout.strip()
        except subprocess.CalledProcessError:
            pass

        return ToolchainInfo(
            name="ios_simulator_sdk",
            is_available=True,
            path=sdk_path,
            version=version,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ToolchainInfo(
            name="ios_simulator_sdk",
            is_available=False,
            error="iOS Simulator SDK not found",
        )


def check_android_ndk() -> ToolchainInfo:
    """Check if Android NDK is available."""
    from pathlib import Path

    # Import the shared NDK detection logic
    from .platform import get_android_ndk_path

    ndk_home = get_android_ndk_path()

    if not ndk_home:
        return ToolchainInfo(
            name="android_ndk",
            is_available=False,
            error="Android NDK not found. Set ANDROID_NDK_HOME or install via Android Studio",
        )

    ndk_path = Path(ndk_home)

    if not ndk_path.exists():
        return ToolchainInfo(
            name="android_ndk",
            is_available=False,
            error=f"NDK path does not exist: {ndk_home}",
        )

    # Check for toolchain file
    toolchain = ndk_path / "build" / "cmake" / "android.toolchain.cmake"
    if not toolchain.exists():
        return ToolchainInfo(
            name="android_ndk",
            is_available=False,
            error="android.toolchain.cmake not found in NDK",
        )

    # Try to get NDK version from source.properties
    version = None
    source_props = ndk_path / "source.properties"
    if source_props.exists():
        try:
            content = source_props.read_text()
            for line in content.split("\n"):
                if line.startswith("Pkg.Revision"):
                    version = line.split("=")[1].strip()
                    break
        except Exception:
            pass

    return ToolchainInfo(
        name="android_ndk",
        is_available=True,
        path=str(ndk_path),
        version=version,
    )


def check_visual_studio() -> ToolchainInfo:
    """Check if Visual Studio is available (Windows only)."""
    import os
    from pathlib import Path

    # Use vswhere to find Visual Studio installations
    # vswhere is typically installed at this path on Windows
    program_files = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    vswhere_path = (
        Path(program_files) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    )

    if not vswhere_path.exists():
        return ToolchainInfo(
            name="visual_studio",
            is_available=False,
            error="vswhere.exe not found. Install Visual Studio 2022 or later.",
        )

    try:
        result = subprocess.run(
            [
                str(vswhere_path),
                "-latest",
                "-products",
                "*",
                "-requires",
                "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-property",
                "installationPath",
            ],
            capture_output=True,
            text=True,
            check=True,
        )
        install_path = result.stdout.strip()

        if not install_path:
            return ToolchainInfo(
                name="visual_studio",
                is_available=False,
                error="Visual Studio with C++ workload not found. Install Visual Studio 2022 with 'Desktop development with C++' workload.",
            )

        # Get VS version
        version = None
        try:
            ver_result = subprocess.run(
                [
                    str(vswhere_path),
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                    "-property",
                    "catalog_productDisplayVersion",
                ],
                capture_output=True,
                text=True,
                check=True,
            )
            version = ver_result.stdout.strip()
        except subprocess.CalledProcessError:
            pass

        # Also try to get the product name (e.g., "Visual Studio Professional 2022")
        product_name = None
        try:
            name_result = subprocess.run(
                [
                    str(vswhere_path),
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                    "-property",
                    "displayName",
                ],
                capture_output=True,
                text=True,
                check=True,
            )
            product_name = name_result.stdout.strip()
        except subprocess.CalledProcessError:
            pass

        version_str = (
            product_name
            if product_name
            else f"Visual Studio {version}"
            if version
            else "Visual Studio"
        )

        return ToolchainInfo(
            name="visual_studio",
            is_available=True,
            path=install_path,
            version=version_str,
        )
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        return ToolchainInfo(
            name="visual_studio",
            is_available=False,
            error=f"Failed to detect Visual Studio: {e}",
        )


def check_windows_sdk() -> ToolchainInfo:
    """Check if Windows SDK is available."""
    import os
    from pathlib import Path

    # Check common Windows SDK locations
    program_files = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    sdk_root = Path(program_files) / "Windows Kits" / "10"

    if not sdk_root.exists():
        return ToolchainInfo(
            name="windows_sdk",
            is_available=False,
            error="Windows SDK not found. Install via Visual Studio Installer or download from Microsoft.",
        )

    # Find the latest SDK version
    include_dir = sdk_root / "Include"
    if not include_dir.exists():
        return ToolchainInfo(
            name="windows_sdk",
            is_available=False,
            error="Windows SDK Include directory not found.",
        )

    # Get available versions (they look like "10.0.19041.0")
    versions = []
    try:
        for item in include_dir.iterdir():
            if item.is_dir() and item.name.startswith("10."):
                versions.append(item.name)
    except OSError:
        pass

    if not versions:
        return ToolchainInfo(
            name="windows_sdk",
            is_available=False,
            error="No Windows SDK versions found in Include directory.",
        )

    # Sort to get the latest version
    versions.sort(reverse=True)
    latest_version = versions[0]

    return ToolchainInfo(
        name="windows_sdk",
        is_available=True,
        path=str(sdk_root),
        version=latest_version,
    )


def get_required_toolchains(targets: List[str]) -> List[str]:
    """Determine which toolchains are required based on target platforms.

    Args:
        targets: List of target strings like 'ios_arm64', 'macos_arm64', 'android_arm64'

    Returns:
        List of required toolchain names
    """
    required = set()

    for target in targets:
        target_lower = target.lower()

        if target_lower.startswith("ios_"):
            required.add("xcode")
            if "simulator" in target_lower or "x86" in target_lower:
                required.add("ios_simulator_sdk")
            else:
                required.add("ios_sdk")

        elif target_lower.startswith("macos_"):
            required.add("xcode")
            required.add("macos_sdk")

        elif target_lower.startswith("android_"):
            required.add("android_ndk")

        elif target_lower.startswith("windows_"):
            required.add("visual_studio")
            required.add("windows_sdk")

    return sorted(required)


def check_toolchains(  # noqa: C901
    targets: List[str], log_level: LogLevel = LogLevel.STATUS
) -> Tuple[bool, List[str]]:
    """Check that required toolchains are available for the given targets.

    Returns a tuple of (all_passed, available_targets) where:
    - all_passed: True if all toolchains for all targets are available
    - available_targets: List of targets that have their required toolchains available

    This allows the build to continue with a subset of targets if some toolchains
    are missing.

    Args:
        targets: List of target strings like 'ios_arm64', 'macos_arm64'
        log_level: Log level for output

    Returns:
        Tuple of (all_passed, available_targets)
    """
    import platform

    system = platform.system().lower()

    if not targets:
        return True, []

    print("Checking toolchains...")
    print()

    # Map toolchain names to check functions
    checkers = {
        "xcode": check_xcode,
        "macos_sdk": check_macos_sdk,
        "ios_sdk": check_ios_sdk,
        "ios_simulator_sdk": check_ios_simulator_sdk,
        "android_ndk": check_android_ndk,
        "visual_studio": check_visual_studio,
        "windows_sdk": check_windows_sdk,
    }

    # Check each toolchain once and cache results
    toolchain_status: Dict[str, Tuple[bool, Optional[str], Optional[str]]] = {}

    for toolchain_name, checker in checkers.items():
        # Skip non-applicable toolchains for this platform
        if toolchain_name in ("xcode", "macos_sdk", "ios_sdk", "ios_simulator_sdk"):
            if system != "darwin":
                toolchain_status[toolchain_name] = (
                    False,
                    None,
                    "Only available on macOS",
                )
                continue

        if toolchain_name in ("visual_studio", "windows_sdk"):
            if system != "windows":
                toolchain_status[toolchain_name] = (
                    False,
                    None,
                    "Only available on Windows",
                )
                continue

        info = checker()
        toolchain_status[toolchain_name] = (info.is_available, info.version, info.error)

    # Determine which targets are available
    available_targets = []
    skipped_targets: Dict[str, List[str]] = {}  # reason -> list of targets

    for target in targets:
        required = get_required_toolchains([target])
        missing = []

        for tc in required:
            is_available, version, error = toolchain_status.get(
                tc, (False, None, "Unknown")
            )
            if not is_available:
                missing.append((tc, error))

        if missing:
            # Group skipped targets by the missing toolchain
            for tc, _error in missing:
                key = f"{tc}"
                if key not in skipped_targets:
                    skipped_targets[key] = []
                skipped_targets[key].append(target)
        else:
            available_targets.append(target)

    # Report available toolchains
    reported_toolchains = set()
    for target in targets:
        for tc in get_required_toolchains([target]):
            if tc not in reported_toolchains:
                reported_toolchains.add(tc)
                is_available, version, error = toolchain_status.get(
                    tc, (False, None, "Unknown")
                )
                if is_available:
                    version_str = f" ({version})" if version else ""
                    print(f"  ✓ {tc}{version_str}")
                else:
                    print(f"  ✗ {tc}: {error}")
                    _print_install_instructions(tc)

    print()

    # Report skipped targets
    if skipped_targets:
        for tc, skipped in skipped_targets.items():
            target_list = ", ".join(skipped)
            print(f"  ⚠ Skipping targets due to missing {tc}: {target_list}")
        print()

    all_passed = len(available_targets) == len(targets)

    if not available_targets:
        print("No targets available - all required toolchains are missing.")
        return False, []

    if not all_passed:
        print(f"Continuing with {len(available_targets)} of {len(targets)} targets.")
        print()

    return all_passed, available_targets
