# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Builder for pre-built (imported) shared libraries.

Used for libraries that ship as pre-built binaries (e.g., AAR files from
Google Maven) rather than being compiled from source.
"""

from __future__ import annotations

import shutil

from lib.builder_base import BuildContext, Builder
from lib.directories import remove_tree
from lib.platform import Arch


# Android ABI mapping (same as in cmake.py)
_ANDROID_ABI_MAP = {
    Arch.ARM64: "arm64-v8a",
    Arch.ARMV7: "armeabi-v7a",
    Arch.X86_64: "x86_64",
    Arch.X86: "x86",
}


class ImportedSharedBuilder(Builder):
    """Builder for pre-built shared libraries.

    This builder handles libraries that are distributed as pre-built
    shared libraries (e.g., .so files extracted from Android AAR packages).

    Build options:
        include_subdir: Subdirectory within source containing headers (default: "include")
        lib_subdir: Subdirectory within source containing shared libraries.
                    Supports "{android_abi}" placeholder for Android ABI names
                    (e.g., "jni/{android_abi}" -> "jni/arm64-v8a").
        lib_files: List of specific library filenames to copy.
                   If not set, all .so/.dylib/.dll files in lib_subdir are copied.
    """

    def configure(self, ctx: BuildContext) -> None:
        """No configuration needed for imported libraries."""
        pass

    def build(self, ctx: BuildContext) -> None:
        """No build needed for imported libraries."""
        pass

    def install(self, ctx: BuildContext) -> None:
        """Copy pre-built shared libraries and headers to install directory."""
        self._ensure_dir(ctx.install_dir)

        # --- Install headers ---
        include_subdir = ctx.build_options.get("include_subdir", "include")
        src_include = ctx.source_dir / include_subdir

        if src_include.exists():
            dst_include = ctx.install_dir / "include"
            if dst_include.exists():
                remove_tree(dst_include)
            shutil.copytree(src_include, dst_include)

        # --- Install shared libraries ---
        lib_subdir = ctx.build_options.get("lib_subdir", "lib")

        # Expand {android_abi} placeholder
        android_abi = _ANDROID_ABI_MAP.get(ctx.target.arch, "")
        lib_subdir = lib_subdir.replace("{android_abi}", android_abi)

        src_lib = ctx.source_dir / lib_subdir

        if not src_lib.exists():
            raise RuntimeError(
                f"Library directory not found: {src_lib}\n"
                f"  source_dir: {ctx.source_dir}\n"
                f"  lib_subdir: {lib_subdir}"
            )

        dst_lib = ctx.install_dir / "lib"
        dst_lib.mkdir(parents=True, exist_ok=True)

        lib_files = ctx.build_options.get("lib_files")
        if lib_files:
            # Copy specific files
            for filename in lib_files:
                src_file = src_lib / filename
                if not src_file.exists():
                    raise RuntimeError(f"Library file not found: {src_file}")
                shutil.copy2(src_file, dst_lib / filename)
        else:
            # Copy all shared library files
            shared_extensions = {".so", ".dylib", ".dll"}
            for item in src_lib.iterdir():
                if item.is_file() and item.suffix in shared_extensions:
                    shutil.copy2(item, dst_lib / item.name)

        if not ctx.progress_callback:
            print(f"    Installed pre-built shared libraries from {lib_subdir}/")
