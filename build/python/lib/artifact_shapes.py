# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Recognising library artifacts from their filename and layout.

`Path.suffix` is the obvious way to classify a library and the wrong one, in
three separate ways this build system has hit:

- `libfoo.so.1.2.3` has suffix `.3`. The version is not a file extension.
- `libfoo.1.2.dylib` puts the version before the extension instead of after.
- A macOS framework binary, `Foo.framework/Versions/A/Foo`, has no extension
  at all.

The rules live here so the audit tool and the imported-shared builder cannot
drift apart on them again. Stdlib only, so a caller can load this module by
path without pulling in the rest of the build system.

Deliberately *not* handled here: telling a Windows static library from a DLL's
import library. Both are `.lib`, and the only reliable answer is to read the
COFF archive -- see `classify_windows_lib()` in `audit_link_variants.py`.
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Optional

# `libfoo.so.1`, `libfoo.so.1.2.3`
_VERSIONED_SO = re.compile(r"\.so(\.\d+)+$")
# `libfoo.1.dylib`, `libfoo.1.2.3.dylib`
_VERSIONED_DYLIB = re.compile(r"\.(\d+)(\.\d+)*\.dylib$")

SHARED_SUFFIXES = frozenset({".dylib", ".so", ".dll"})
STATIC_SUFFIXES = frozenset({".a", ".lib"})


def is_versioned_shared_name(name: str) -> bool:
    """Whether the name carries a version where a suffix check would miss it."""
    return bool(_VERSIONED_SO.search(name) or _VERSIONED_DYLIB.search(name))


def is_shared_library_name(name: str) -> bool:
    """Whether the filename names a shared library, versioned or not."""
    return is_versioned_shared_name(name) or Path(name).suffix in SHARED_SUFFIXES


def is_static_library_name(name: str) -> bool:
    """Whether the filename names a static library.

    On Windows this is true for an import library too; the caller has to read
    the archive to tell them apart.
    """
    return Path(name).suffix in STATIC_SUFFIXES


def strip_version(name: str) -> str:
    """Drop a shared-library version, leaving the base name.

    `libz.so.1.3.1` and `libz.1.3.1.dylib` both reduce to `libz`.
    """
    for pattern in (_VERSIONED_SO, _VERSIONED_DYLIB):
        name = pattern.sub("", name)
    return name


def target_without_link_type(target: str) -> str:
    """A build-target name with its link-type component removed.

    `macos_arm64_static_debug` -> `macos_arm64_debug`. Callers use this to pair
    the two link-type variants of one target, so everything else -- OS,
    architecture, configuration, MSVC toolset -- has to survive it.
    """
    return "_".join(
        part for part in target.split("_") if part not in ("static", "shared")
    )


def framework_binary_name(path: Path) -> Optional[str]:
    """The framework name if `path` is a framework's binary, else None.

    Recognises both layouts Apple defines:

        Foo.framework/Foo                     (flat, used on iOS)
        Foo.framework/Versions/A/Foo          (versioned, used on macOS)

    A framework binary is a shared library with no extension, so nothing else
    in this module can identify it. Only the binary matching the bundle name
    qualifies -- headers, resources and helper executables inside the bundle
    are not the library.
    """
    parts = path.parts
    for index, part in enumerate(parts):
        if not part.endswith(".framework"):
            continue
        name = part[: -len(".framework")]
        remainder = parts[index + 1 :]
        if remainder == (name,):
            return name
        if len(remainder) == 3 and remainder[0] == "Versions" and remainder[2] == name:
            return name
    return None
