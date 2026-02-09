# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Ocean Third-Party Build System - Core Library."""

from .dag import DependencyGraph, print_build_plan
from .directories import BuildPaths, DirectoryManager
from .fetcher import SourceFetcher
from .manifest import (
    BuildConfig as ManifestBuildConfig,
    LibraryConfig,
    Manifest,
    ManifestDefaults,
    SourceConfig,
)
from .platform import (
    Arch,
    BuildConfig,
    BuildTarget,
    detect_host_arch,
    detect_host_os,
    detect_host_target,
    get_msvc_toolset_version,
    LinkType,
    OS,
    parse_platform_string,
    parse_target_string,
)
from .preflight import check_toolchains, LogLevel, run_preflight_checks

__all__ = [
    # DAG
    "DependencyGraph",
    "print_build_plan",
    # Directories
    "BuildPaths",
    "DirectoryManager",
    # Fetcher
    "SourceFetcher",
    # Manifest
    "LibraryConfig",
    "Manifest",
    "ManifestBuildConfig",
    "ManifestDefaults",
    "SourceConfig",
    # Platform
    "Arch",
    "BuildConfig",
    "BuildTarget",
    "LinkType",
    "OS",
    "detect_host_arch",
    "detect_host_os",
    "detect_host_target",
    "get_msvc_toolset_version",
    "parse_platform_string",
    "parse_target_string",
    # Preflight
    "LogLevel",
    "check_toolchains",
    "run_preflight_checks",
]
