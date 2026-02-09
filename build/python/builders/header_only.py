# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Header-only library builder."""

from __future__ import annotations

import shutil

from lib.builder_base import BuildContext, Builder


class HeaderOnlyBuilder(Builder):
    """Builder for header-only libraries.

    Simply copies the include directory to the install location.
    No configuration or compilation is needed.
    """

    def configure(self, ctx: BuildContext) -> None:
        """No configuration needed for header-only libraries."""
        pass

    def build(self, ctx: BuildContext) -> None:
        """No build needed for header-only libraries."""
        pass

    def install(self, ctx: BuildContext) -> None:
        """Copy headers to install directory."""
        self._ensure_dir(ctx.install_dir)

        # Determine include source directory
        include_subdir = ctx.build_options.get("include_subdir", "include")
        src_include = ctx.source_dir / include_subdir

        if not src_include.exists():
            raise RuntimeError(f"Include directory not found: {src_include}")

        # Copy to install location
        dst_include = ctx.install_dir / "include"
        if dst_include.exists():
            shutil.rmtree(dst_include)

        shutil.copytree(src_include, dst_include)

        # Only print if not in TUI mode
        if not ctx.progress_callback:
            print(f"    Copied headers from {include_subdir}/")
