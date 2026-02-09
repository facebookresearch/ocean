# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""CMake build system adapter."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path
from typing import List, Optional

from lib.builder_base import BuildContext, Builder
from lib.platform import (
    Arch,
    BuildConfig,
    BuildTarget,
    get_android_ndk_path,
    get_cmake_generator,
    get_ios_sdk_path,
    LinkType,
    OS,
)
from lib.preflight import LogLevel


class CMakeBuilder(Builder):
    """Builder for CMake-based projects."""

    def configure(self, ctx: BuildContext) -> None:
        """Configure CMake project."""
        self._ensure_dir(ctx.build_dir)

        cmd = self._build_configure_command(ctx)
        quiet = ctx.progress_callback is not None
        self._run(
            cmd,
            cwd=ctx.build_dir,
            log_level=ctx.log_level,
            quiet=quiet,
            log_file=ctx.log_file,
            phase="configure",
        )

    def build(self, ctx: BuildContext) -> None:
        """Build the project."""
        jobs = ctx.jobs if ctx.jobs > 0 else os.cpu_count() or 4

        cmd = [
            "cmake",
            "--build",
            str(ctx.build_dir),
            "--parallel",
            str(jobs),
        ]

        # Add config for multi-config generators (Visual Studio, Xcode)
        generator = get_cmake_generator(ctx.target, ctx.vs_version)
        if "Visual Studio" in generator or generator == "Xcode":
            config = (
                "Debug" if ctx.target.build_config == BuildConfig.DEBUG else "Release"
            )
            cmd.extend(["--config", config])

        quiet = ctx.progress_callback is not None
        self._run(
            cmd,
            log_level=ctx.log_level,
            quiet=quiet,
            log_file=ctx.log_file,
            phase="build",
        )

    def install(self, ctx: BuildContext) -> None:
        """Install the built artifacts."""
        self._ensure_dir(ctx.install_dir)

        cmd = [
            "cmake",
            "--install",
            str(ctx.build_dir),
            "--prefix",
            str(ctx.install_dir),
        ]

        # Add config for multi-config generators
        generator = get_cmake_generator(ctx.target, ctx.vs_version)
        if "Visual Studio" in generator or generator == "Xcode":
            config = (
                "Debug" if ctx.target.build_config == BuildConfig.DEBUG else "Release"
            )
            cmd.extend(["--config", config])

        quiet = ctx.progress_callback is not None
        self._run(
            cmd,
            log_level=ctx.log_level,
            quiet=quiet,
            log_file=ctx.log_file,
            phase="install",
        )

    def _build_configure_command(self, ctx: BuildContext) -> List[str]:
        """Build the cmake configure command."""
        cmd = ["cmake"]

        # Generator
        generator = get_cmake_generator(ctx.target, ctx.vs_version)
        cmd.extend(["-G", generator])

        # Build type (for single-config generators)
        if "Visual Studio" not in generator and generator != "Xcode":
            build_type = (
                "Debug" if ctx.target.build_config == BuildConfig.DEBUG else "Release"
            )
            cmd.append(f"-DCMAKE_BUILD_TYPE={build_type}")

        # Install prefix
        cmd.append(f"-DCMAKE_INSTALL_PREFIX={ctx.install_dir}")

        # Static vs shared
        if ctx.target.link_type == LinkType.STATIC:
            cmd.append("-DBUILD_SHARED_LIBS=OFF")
        else:
            cmd.append("-DBUILD_SHARED_LIBS=ON")

        # Add dependency paths
        cmake_prefix_paths = [
            str(ctx.get_dependency_cmake_prefix(dep))
            for dep in ctx.dependency_dirs
            if ctx.get_dependency_cmake_prefix(dep)
        ]
        if cmake_prefix_paths:
            cmd.append(f"-DCMAKE_PREFIX_PATH={';'.join(cmake_prefix_paths)}")

        # Add explicit include and library paths for Ocean's non-standard layout
        # Ocean uses h/<platform>/ for headers and lib/<target>/ for libraries
        # CMake's FindXXX modules expect include/ and lib/, so we add explicit paths
        include_paths = []
        library_paths = []
        for dep in ctx.dependency_dirs:
            inc_dir = ctx.get_dependency_include_dir(dep)
            if inc_dir:
                include_paths.append(str(inc_dir))
            lib_dir = ctx.get_dependency_lib_dir(dep)
            if lib_dir:
                library_paths.append(str(lib_dir))

        if include_paths:
            cmd.append(f"-DCMAKE_INCLUDE_PATH={';'.join(include_paths)}")
        if library_paths:
            cmd.append(f"-DCMAKE_LIBRARY_PATH={';'.join(library_paths)}")

        # Cross-compilation settings
        self._add_cross_compile_options(cmd, ctx.target)

        # Add user-specified options from manifest
        for key, value in ctx.build_options.items():
            if isinstance(value, bool):
                cmd.append(f"-D{key}={'ON' if value else 'OFF'}")
            else:
                cmd.append(f"-D{key}={value}")

        # Source directory
        cmd.append(str(ctx.source_dir))

        return cmd

    def _add_cross_compile_options(self, cmd: List[str], target: BuildTarget) -> None:
        """Add cross-compilation options for the target."""
        if target.os == OS.ANDROID:
            self._add_android_options(cmd, target)
        elif target.os == OS.IOS:
            self._add_ios_options(cmd, target)
        elif target.os == OS.MACOS:
            self._add_macos_options(cmd, target)
        elif target.os == OS.WINDOWS:
            self._add_windows_options(cmd, target)
        # Linux native builds don't need special handling

    def _add_android_options(self, cmd: List[str], target: BuildTarget) -> None:
        """Add Android NDK toolchain options."""
        ndk_path = get_android_ndk_path()
        if not ndk_path:
            raise RuntimeError(
                "Android NDK not found. Set ANDROID_NDK_HOME environment variable."
            )

        toolchain = Path(ndk_path) / "build" / "cmake" / "android.toolchain.cmake"
        if not toolchain.exists():
            raise RuntimeError(f"Android toolchain not found: {toolchain}")

        cmd.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain}")

        # Set Android ABI
        abi_map = {
            Arch.ARM64: "arm64-v8a",
            Arch.ARMV7: "armeabi-v7a",
            Arch.X86_64: "x86_64",
            Arch.X86: "x86",
        }
        cmd.append(f"-DANDROID_ABI={abi_map[target.arch]}")

        # API level (minimum supported Android version)
        cmd.append("-DANDROID_PLATFORM=android-24")

        # Use libc++ (modern C++ library)
        cmd.append("-DANDROID_STL=c++_static")

    def _add_ios_options(self, cmd: List[str], target: BuildTarget) -> None:
        """Add iOS cross-compilation options."""
        # Use the ios-cmake toolchain if available, otherwise set manually
        toolchain_path = self._find_ios_toolchain()

        if toolchain_path:
            cmd.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain_path}")

            # Platform selection
            if target.arch == Arch.ARM64:
                cmd.append("-DPLATFORM=OS64")  # iOS device
            else:
                cmd.append("-DPLATFORM=SIMULATOR64")  # iOS simulator

            cmd.append("-DDEPLOYMENT_TARGET=15.0")

            # Enable symbol visibility for shared library builds
            # The ios-cmake toolchain defaults to -fvisibility=hidden which
            # hides all symbols. For shared libraries, we need symbols exported.
            if target.link_type == LinkType.SHARED:
                cmd.append("-DENABLE_VISIBILITY=ON")
        else:
            # Manual configuration
            sdk_path = get_ios_sdk_path("iphoneos")
            if not sdk_path:
                raise RuntimeError("iOS SDK not found")

            cmd.extend(
                [
                    "-DCMAKE_SYSTEM_NAME=iOS",
                    f"-DCMAKE_OSX_SYSROOT={sdk_path}",
                    "-DCMAKE_OSX_DEPLOYMENT_TARGET=15.0",
                ]
            )

            if target.arch == Arch.ARM64:
                cmd.append("-DCMAKE_OSX_ARCHITECTURES=arm64")

    def _add_macos_options(self, cmd: List[str], target: BuildTarget) -> None:
        """Add macOS options."""
        cmd.append("-DCMAKE_OSX_DEPLOYMENT_TARGET=12.0")

        if target.arch == Arch.ARM64:
            cmd.append("-DCMAKE_OSX_ARCHITECTURES=arm64")
            cmd.append("-DCMAKE_SYSTEM_PROCESSOR=arm64")
        elif target.arch == Arch.X86_64:
            cmd.append("-DCMAKE_OSX_ARCHITECTURES=x86_64")
            cmd.append("-DCMAKE_SYSTEM_PROCESSOR=x86_64")

    def _add_windows_options(self, cmd: List[str], target: BuildTarget) -> None:
        """Add Windows options including architecture for Visual Studio generator.

        Visual Studio generators require the -A flag to specify target architecture.
        This enables cross-compilation (e.g., building ARM64 on x64 host).
        """
        # Map Ocean Arch to Visual Studio architecture names
        arch_map = {
            Arch.ARM64: "ARM64",
            Arch.X86_64: "x64",
            Arch.X86: "Win32",
        }

        if target.arch not in arch_map:
            raise RuntimeError(f"Unsupported Windows architecture: {target.arch}")

        # Add architecture flag for Visual Studio generator
        cmd.append(f"-A {arch_map[target.arch]}")

    def _find_ios_toolchain(self) -> Optional[Path]:
        """Find iOS CMake toolchain file."""
        # Look for toolchain in our local toolchains directory
        toolchain_path = (
            Path(__file__).parent.parent / "toolchains" / "ios.toolchain.cmake"
        )

        if toolchain_path.exists():
            return toolchain_path

        return None

    @staticmethod
    def _run(  # noqa: C901
        cmd: List[str],
        cwd: Optional[Path] = None,
        log_level: Optional[LogLevel] = None,
        quiet: bool = False,
        log_file: Optional[Path] = None,
        phase: str = "",
    ) -> subprocess.CompletedProcess:
        """Run a command.

        Args:
            cmd: Command to run
            cwd: Working directory
            log_level: Log level for output control
            quiet: If True, suppress all output except errors (for TUI mode)
            log_file: Path to log file for build output
            phase: Build phase name for log file headers
        """
        import datetime

        if log_level is None:
            log_level = LogLevel.STATUS

        # Show command based on log level (unless quiet mode)
        if not quiet:
            if log_level >= LogLevel.VERBOSE:
                print(f"    $ {' '.join(cmd)}")
            elif log_level >= LogLevel.STATUS:
                print(f"    $ {' '.join(cmd[:4])}...")

        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
        )

        # Write to log file if specified
        if log_file:
            try:
                log_file.parent.mkdir(parents=True, exist_ok=True)
                with open(log_file, "a") as f:
                    timestamp = datetime.datetime.now().isoformat()
                    f.write(f"\n{'=' * 60}\n")
                    f.write(f"[{timestamp}] Phase: {phase}\n")
                    f.write(f"Command: {' '.join(cmd)}\n")
                    if cwd:
                        f.write(f"Working directory: {cwd}\n")
                    f.write(f"{'=' * 60}\n\n")
                    if result.stdout:
                        f.write("=== STDOUT ===\n")
                        f.write(result.stdout)
                        f.write("\n")
                    if result.stderr:
                        f.write("=== STDERR ===\n")
                        f.write(result.stderr)
                        f.write("\n")
                    f.write(f"\nExit code: {result.returncode}\n")
            except Exception:
                # Don't fail the build if logging fails
                pass

        # Show output based on log level (unless quiet mode)
        if not quiet:
            if log_level >= LogLevel.DEBUG:
                if result.stdout:
                    print(result.stdout)
                if result.stderr:
                    print(result.stderr)
            elif log_level >= LogLevel.VERBOSE and result.returncode == 0:
                # Show last few lines on success in verbose mode
                if result.stdout:
                    lines = result.stdout.strip().split("\n")
                    if len(lines) > 5:
                        print(f"    ... ({len(lines) - 5} lines omitted)")
                    for line in lines[-5:]:
                        print(f"    {line}")

        if result.returncode != 0:
            print(
                f"    STDOUT: {result.stdout[-2000:] if len(result.stdout) > 2000 else result.stdout}"
            )
            print(
                f"    STDERR: {result.stderr[-2000:] if len(result.stderr) > 2000 else result.stderr}"
            )
            raise subprocess.CalledProcessError(
                result.returncode, cmd, result.stdout, result.stderr
            )

        return result
