#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Report third-party producers that install more than one public link variant.

The packaging step drops wrong-link-type artifacts after `cmake --install`,
which is unsound: it infers intent from a file extension. On Windows a static
library and a DLL's import library are both `.lib`, so filtering a static
target deleted the DLL and kept its import library, publishing a prefix that
links and then fails at process start. Filtering is now disabled there.

The durable fix is for each producer to install only the variant that was
asked for, the way the zlib, libyuv and mbedTLS patches already do. This tool
produces the list of producers that do not, so the remaining filter can
eventually be removed as a provable no-op rather than a behaviour change.

It inspects the *unfiltered* staging tree (`<build>/<lib>/<version>/<target>/
_install`), i.e. exactly what `cmake --install` wrote, before packaging has
touched it.

Usage:
    # Populate staging trees first, once per link type
    ./build_ocean_3rdparty.py --target macos_arm64 --config release --link static
    ./build_ocean_3rdparty.py --target macos_arm64 --config release --link shared

    # Then audit them
    ./audit_link_variants.py --build-dir ocean_3rdparty/build
    ./audit_link_variants.py --build-dir ocean_3rdparty/build --json audit.json
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

SHARED_SUFFIXES = {".dylib", ".so", ".dll"}
STATIC_SUFFIXES = {".a", ".lib"}

# `libfoo.so.1.2.3` and `libfoo.1.2.3.dylib` are shared libraries whose suffix
# is not the last component.
_VERSIONED_SO = re.compile(r"\.so(\.\d+)+$")
_VERSIONED_DYLIB = re.compile(r"\.(\d+)(\.\d+)*\.dylib$")

# Trailing markers producers use to name the alternate variant of the same
# library. Only used to pair artifacts for reporting -- never to delete
# anything, because the convention is not reliable.
#
# Longest first, so `_static` is consumed before `static` would leave a stray
# separator. MSVC's bare `s` suffix is deliberately absent: it strips the last
# letter of every library whose name ends in one, so `mbedtls.dll` reduced to
# `mbedtl` while `mbedtls_static.lib` reduced to `mbedtls` -- splitting exactly
# the pair this function exists to join.
_VARIANT_MARKERS = ("_static", "-static", "static", "_shared", "-shared", "shared")


class LibKind:
    STATIC = "static"
    SHARED = "shared"
    IMPORT = "import"
    UNKNOWN = "unknown"


def _iter_archive_members(handle):
    """Yield each ar member's payload.

    Raises ValueError if the archive is malformed or truncated. A partial
    header, an unparseable size, or a payload shorter than its header claims
    all mean the file cannot be trusted -- and an untrustworthy archive must
    not be reported as a static library, because that is the answer that lets
    a missing DLL through.
    """
    while True:
        header = handle.read(60)
        if not header:
            return
        if len(header) < 60:
            raise ValueError("truncated archive member header")
        try:
            size = int(header[48:58].decode("ascii").strip())
        except ValueError as exc:
            raise ValueError("malformed archive member header") from exc
        if size < 0:
            # read(-1) means "read to EOF", which would swallow the rest of the
            # archive as one member and make the short-payload check below
            # compare against a negative number -- so a single corrupt size
            # field would yield a confident STATIC.
            raise ValueError("negative archive member size")
        data = handle.read(size)
        if len(data) < size:
            raise ValueError("truncated archive member payload")
        yield data
        if size % 2:
            handle.seek(1, 1)


def _short_import_dll(data: bytes) -> Optional[str]:
    """DLL named by a short import object, or None if this is not one."""
    if data[:4] != b"\x00\x00\xff\xff":
        return None
    # IMPORT_OBJECT_HEADER is 20 bytes; then the null-terminated symbol name
    # and the null-terminated DLL name.
    parts = data[20:].split(b"\x00")
    if len(parts) >= 2 and parts[1]:
        return parts[1].decode("ascii", "replace")
    return ""


def _old_style_import_dll(data: bytes) -> Optional[str]:
    """DLL named by a pre-short-format import member, or None if not one.

    Returns "" when the member proves this is an import library but does not
    name the DLL.
    """
    if b"__IMPORT_DESCRIPTOR_" not in data and b"__NULL_IMPORT_DESCRIPTOR" not in data:
        return None
    match = re.search(rb"__IMPORT_DESCRIPTOR_([\w.+-]+)", data)
    if not match:
        return ""
    base = match.group(1).decode("ascii", "replace")
    return base if "." in base else f"{base}.dll"


def classify_windows_lib(path: Path) -> Tuple[str, Optional[str]]:
    """Tell a Windows import library from a static one by reading the archive.

    Both are COFF archives with the same extension. A modern import library
    contains "short import object" members whose data begins with the
    IMAGE_FILE_MACHINE_UNKNOWN / 0xFFFF signature; a static archive holds real
    COFF objects and never does. The short import record also names the DLL it
    describes, which makes the import-library/DLL pairing exact rather than a
    filename guess.

    Old-style import libraries predate the short format. They are still
    detectable, by the `__IMPORT_DESCRIPTOR_<dll>` and
    `__NULL_IMPORT_DESCRIPTOR` symbols the linker emits into them, which a
    static archive never carries. An archive matching neither shape is a
    static library.

    A truncated or non-archive file is UNKNOWN rather than a guess. Reporting
    only; nothing is deleted on the strength of this.

    Returns:
        (kind, dll_name). dll_name is set for a detected import library when
        the format records it.
    """
    try:
        with open(path, "rb") as handle:
            if handle.read(8) != b"!<arch>\n":
                return LibKind.UNKNOWN, None
            saw_short_import = False
            short_dll: Optional[str] = None
            saw_old_style = False
            old_style_dll: Optional[str] = None
            try:
                # Every member is walked even once an import record is found.
                # Returning early would classify an archive whose later members
                # are damaged, and an IMPORT verdict is not a neutral one: if
                # the named DLL happens to be present the tree then reports OK
                # and passes the filter-removal gate, on the strength of a file
                # that was never fully read.
                for data in _iter_archive_members(handle):
                    if saw_short_import:
                        continue
                    short = _short_import_dll(data)
                    if short is not None:
                        saw_short_import = True
                        short_dll = short or None
                        continue
                    named = _old_style_import_dll(data)
                    if named is not None:
                        saw_old_style = True
                        old_style_dll = old_style_dll or named or None
            except ValueError:
                return LibKind.UNKNOWN, None
            if saw_short_import:
                return LibKind.IMPORT, short_dll
            if saw_old_style:
                return LibKind.IMPORT, old_style_dll
            return LibKind.STATIC, None
    except OSError:
        return LibKind.UNKNOWN, None


def artifact_kind(path: Path, is_windows: bool) -> Tuple[Optional[str], Optional[str]]:
    """Classify one file, returning (kind, dll_name); (None, None) if not a library."""
    name = path.name
    if _VERSIONED_SO.search(name) or _VERSIONED_DYLIB.search(name):
        return LibKind.SHARED, None
    suffix = path.suffix
    if suffix in SHARED_SUFFIXES:
        return LibKind.SHARED, None
    if suffix == ".lib":
        if is_windows:
            return classify_windows_lib(path)
        return LibKind.STATIC, None
    if suffix == ".a":
        return LibKind.STATIC, None
    return None, None


def public_stem(name: str) -> str:
    """Reduce an artifact filename to the library it is a variant of.

    `libz.a`, `libz.1.3.1.dylib` and `zlibstatic.lib` all reduce to something
    comparable, so the two public variants of one library can be paired.

    Everything is done on a case-folded name. Windows filenames are
    case-insensitive and producers are inconsistent about it, so comparing
    `FOO.DLL` against `foo.dll` by exact case silently puts one library into
    two groups -- and a split pair is not reported as the dual-variant case it
    actually is.

    A wrong split is the failure mode that matters: it demotes a real
    DUAL_PUBLIC to REVIEW, which the report invites the reader to allowlist.
    Over-merging two genuinely different libraries only produces a spurious
    report, so the order below deliberately errs towards merging.
    """
    stem = name.casefold()
    for pattern in (_VERSIONED_SO, _VERSIONED_DYLIB):
        stem = pattern.sub("", stem)
    for suffix in (".dylib", ".so", ".dll", ".lib", ".a"):
        if stem.endswith(suffix):
            stem = stem[: -len(suffix)]
            break
    if stem.startswith("lib"):
        stem = stem[3:]
    for marker in _VARIANT_MARKERS:
        if stem.endswith(marker) and len(stem) > len(marker):
            stem = stem[: -len(marker)]
            break
    # Separator first, then digits, then separator again. zlib ships
    # `zlibstatic.lib` beside `zlib1.dll`, and libpng ships `libpng16.dll`
    # beside `libpng16_static.lib` -- the latter only pairs if the `_` left by
    # the marker strip is removed before the digits are, otherwise one side
    # keeps its version number and the pair splits.
    stem = stem.rstrip("-_")
    stem = re.sub(r"\d+$", "", stem)
    return stem.rstrip("-_")


@dataclass
class Finding:
    library: str
    target: str
    requested: str
    verdict: str
    detail: str
    artifacts: Dict[str, List[str]] = field(default_factory=dict)
    dangling_metadata: List[str] = field(default_factory=list)


def _collect(
    staging: Path, is_windows: bool
) -> Tuple[Dict[str, List[Tuple[Path, str]]], Dict[str, str]]:
    """Group library artifacts by the library they are a variant of.

    An import library is grouped with the DLL it names rather than by its own
    filename, because the two are routinely spelled differently -- zlib ships
    `zlib.lib` for `zlib1.dll`. Returns the grouping and, for Windows, the
    import-library -> DLL mapping so unpaired ones can be reported.
    """
    grouped: Dict[str, List[Tuple[Path, str]]] = {}
    imports: Dict[str, str] = {}
    for path in sorted(staging.rglob("*")):
        if not path.is_file() or path.is_symlink():
            continue
        kind, dll = artifact_kind(path, is_windows)
        if kind is None:
            continue
        stem = (
            public_stem(dll)
            if (kind == LibKind.IMPORT and dll)
            else public_stem(path.name)
        )
        grouped.setdefault(stem, []).append((path, kind))
        if kind == LibKind.IMPORT:
            imports[path.name] = dll or "<unknown>"
    return grouped, imports


def _dangling_metadata(staging: Path) -> List[str]:
    """CMake package files referencing a library file that is not in the tree.

    A tree can look coherent while its exported targets still point at the
    variant that was filtered away, so the metadata is checked separately.
    """
    dangling: List[str] = []
    present = {p.name for p in staging.rglob("*") if p.is_file() or p.is_symlink()}
    for cmake_file in staging.rglob("*.cmake"):
        try:
            text = cmake_file.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for referenced in re.findall(
            r"[\w.+-]+\.(?:so(?:\.\d+)*|dylib|dll|lib|a)\b", text
        ):
            base = Path(referenced).name
            if base not in present and not base.startswith("$"):
                entry = f"{cmake_file.name} -> {base}"
                if entry not in dangling:
                    dangling.append(entry)
    return dangling


def _classify_groups(
    grouped: Dict[str, List[Tuple[Path, str]]], requested: str
) -> Tuple[List[str], List[str], List[str], List[str], Dict[str, List[str]]]:
    """Sort each library group by what it installed.

    Returns (dual, orphan_static, wrong_only, unclassified, artifacts).
    """
    dual: List[str] = []
    orphan_static: List[str] = []
    wrong_only: List[str] = []
    unclassified: List[str] = []
    artifacts: Dict[str, List[str]] = {}

    for stem, entries in grouped.items():
        kinds = {kind for _, kind in entries}
        artifacts[stem] = [f"{path.name} [{kind}]" for path, kind in entries]

        # An archive we could not read is not evidence of anything. Saying
        # "single public variant" about it would let the filter-removal gate
        # pass on a tree nobody has actually classified.
        if LibKind.UNKNOWN in kinds:
            unclassified.append(stem)
            continue

        # A DLL and its import library are one shared artifact, so an import
        # library counts as a shared variant being present even when the DLL
        # itself was installed elsewhere in the tree.
        real_static = LibKind.STATIC in kinds
        real_shared = LibKind.SHARED in kinds or LibKind.IMPORT in kinds

        if real_static and real_shared:
            dual.append(stem)
        elif real_static and requested == LibKind.SHARED:
            orphan_static.append(stem)
        elif real_shared and not real_static and requested == LibKind.STATIC:
            # Only the variant that was not asked for. The filter would have
            # deleted it and published nothing, so this must be fixed at the
            # producer before filtering can be removed.
            wrong_only.append(stem)

    return dual, orphan_static, wrong_only, unclassified, artifacts


def audit_staging(staging: Path, library: str, target: str, requested: str) -> Finding:
    """Classify one library's unfiltered install tree."""
    is_windows = target.startswith("win")
    grouped, imports = _collect(staging, is_windows)
    present = {p.name for p in staging.rglob("*") if p.is_file() or p.is_symlink()}

    broken_pairs: List[str] = []
    dual, orphan_static, wrong_only, unclassified, artifacts = _classify_groups(
        grouped, requested
    )
    # An import library whose DLL is missing links cleanly and then fails at
    # process start. That is the state the old Windows filter manufactured.
    # Compared case-insensitively: the PE import table and the installed
    # filename routinely differ in case, and Windows does not care.
    present_folded = {name.casefold() for name in present}
    for lib_name, dll in sorted(imports.items()):
        if dll and dll != "<unknown>" and dll.casefold() not in present_folded:
            broken_pairs.append(f"{lib_name} imports {dll}, which is not installed")

    dangling = _dangling_metadata(staging)

    if broken_pairs:
        verdict = "BROKEN_PAIR"
        detail = "; ".join(broken_pairs)
    elif dual:
        verdict = "DUAL_PUBLIC"
        detail = (
            "installs both public variants for one requested link type: "
            + ", ".join(sorted(dual))
        )
    elif wrong_only:
        verdict = "WRONG_ONLY"
        detail = (
            f"installs only the variant that was not requested ({requested} "
            "was asked for): " + ", ".join(sorted(wrong_only))
        )
    elif unclassified:
        verdict = "UNCLASSIFIED"
        detail = (
            "could not determine what these are; an unreadable archive is not "
            "evidence that the tree is clean: " + ", ".join(sorted(unclassified))
        )
    elif dangling:
        verdict = "DANGLING_METADATA"
        detail = (
            "exported CMake targets reference a library file that is not "
            "installed: " + "; ".join(dangling)
        )
    elif orphan_static:
        verdict = "REVIEW"
        detail = (
            "static archives alongside a shared build; may be legitimate "
            "helpers/plugins rather than public alternates: "
            + ", ".join(sorted(orphan_static))
        )
    elif not grouped:
        verdict = "NO_LIBRARIES"
        detail = "no library artifacts found (header-only, or build produced none)"
    else:
        verdict = "OK"
        detail = "single public variant"

    return Finding(
        library=library,
        target=target,
        requested=requested,
        verdict=verdict,
        detail=detail,
        artifacts=artifacts,
        dangling_metadata=dangling,
    )


def discover(build_dir: Path) -> List[Tuple[Path, str, str]]:
    """Find (staging, library, target) for every _install tree under build_dir."""
    found = []
    for staging in sorted(build_dir.rglob("_install")):
        if not staging.is_dir():
            continue
        # <build>/<library>/<version>/<target>/_install
        try:
            target = staging.parent.name
            library = staging.relative_to(build_dir).parts[0]
        except (IndexError, ValueError):
            continue
        found.append((staging, library, target))
    return found


def _find_missing_coverage(findings: List[Finding]) -> List[str]:
    """Libraries audited for only one link type.

    The question this tool answers -- does a producer install both public
    variants -- cannot be answered from one link type alone. Without this a run
    that only built static prints "every audited tree installs exactly the
    requested variant" and exits 0, which is the same false all-clear the
    verdicts were hardened against.
    """
    seen: Dict[str, set] = {}
    for finding in findings:
        seen.setdefault(finding.library, set()).add(finding.requested)
    return sorted(
        name for name, kinds in seen.items() if {LibKind.STATIC, LibKind.SHARED} - kinds
    )


def report(
    findings: List[Finding],
    build_dir: Path,
    missing_coverage: Optional[List[str]] = None,
) -> List[str]:
    """Print the audit, worst first. Returns the producers that need fixing."""
    order = {
        "BROKEN_PAIR": 0,
        "DUAL_PUBLIC": 1,
        "WRONG_ONLY": 2,
        "UNCLASSIFIED": 3,
        "DANGLING_METADATA": 4,
        "REVIEW": 5,
        "OK": 6,
        "NO_LIBRARIES": 7,
    }
    findings.sort(key=lambda f: (order.get(f.verdict, 9), f.library, f.target))

    print(f"Audited {len(findings)} staging trees under {build_dir}\n")
    for f in findings:
        if f.verdict == "OK":
            continue
        print(f"[{f.verdict}] {f.library} ({f.target}, requested {f.requested})")
        print(f"    {f.detail}")
        for stem, items in sorted(f.artifacts.items()):
            print(f"      {stem}: {', '.join(items)}")
        for entry in f.dangling_metadata:
            print(f"      dangling metadata: {entry}")
        print()

    counts: Dict[str, int] = {}
    for f in findings:
        counts[f.verdict] = counts.get(f.verdict, 0) + 1
    print("Summary: " + ", ".join(f"{v}={c}" for v, c in sorted(counts.items())))

    # Everything except OK and NO_LIBRARIES blocks. REVIEW means "this tree has
    # artifacts we could not classify with confidence" -- removing the filter
    # would not be byte-identical, so an unresolved REVIEW must not read as a
    # clean bill of health.
    blocking = sorted(
        {f.library for f in findings if f.verdict not in ("OK", "NO_LIBRARIES")}
        | set(missing_coverage or [])
    )
    if missing_coverage:
        print(
            "\nAudited for only one link type, so nothing can be concluded about these:"
        )
        for name in missing_coverage:
            print(f"  - {name}")
        print("  Build both --link static and --link shared, or pass --allow-partial.")
    if blocking:
        print("\nProducers needing attention before the filter can be removed:")
        for name in blocking:
            verdicts = sorted(
                {
                    f.verdict
                    for f in findings
                    if f.library == name and f.verdict not in ("OK", "NO_LIBRARIES")
                }
            )
            if name in (missing_coverage or []):
                verdicts.append("ONE_LINK_TYPE_ONLY")
            print(f"  - {name} ({', '.join(verdicts)})")
        print(
            "\nThe post-install link-type filter cannot be removed on macOS/Linux "
            "until this list is empty. A REVIEW entry must be resolved or "
            "explicitly allowlisted, not ignored."
        )
    else:
        print("\nEvery audited tree installs exactly the requested variant.")
    return blocking


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Report producers installing more than one public link variant",
        epilog=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        required=True,
        help="Build directory holding the _install staging trees",
    )
    parser.add_argument(
        "--library", action="append", default=None, help="Limit to these libraries"
    )
    parser.add_argument("--json", type=Path, default=None, help="Also write JSON here")
    parser.add_argument(
        "--allow-partial",
        action="store_true",
        help="Do not require every library to be audited for both link types",
    )
    args = parser.parse_args()

    if not args.build_dir.is_dir():
        print(f"Error: build directory not found: {args.build_dir}")
        return 1

    trees = discover(args.build_dir)
    if args.library:
        wanted = set(args.library)
        trees = [t for t in trees if t[1] in wanted]

    if not trees:
        print(f"No _install staging trees under {args.build_dir}.")
        print("Run build_ocean_3rdparty.py first, once per link type.")
        return 1

    findings = []
    for staging, library, target in trees:
        requested = LibKind.SHARED if "shared" in target else LibKind.STATIC
        findings.append(audit_staging(staging, library, target, requested))

    missing_coverage = [] if args.allow_partial else _find_missing_coverage(findings)
    blocking = report(findings, args.build_dir, missing_coverage)

    if args.json:
        args.json.write_text(
            json.dumps([f.__dict__ for f in findings], indent=2), encoding="utf-8"
        )
        print(f"\nWrote {args.json}")

    return 1 if blocking else 0


if __name__ == "__main__":
    sys.exit(main())
