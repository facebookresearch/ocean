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

    # Audit what is there. This reports on the scope it found and nothing more.
    ./audit_link_variants.py --build-dir ocean_3rdparty/build

    # Evaluate the filter-removal gate. The matrix says which results are
    # supposed to exist, so a platform nobody built is a failure rather than
    # a requirement that quietly disappears with it.
    ./build_ocean_3rdparty.py --target macos_arm64 --emit-expected-matrix matrix.json
    ./audit_link_variants.py --build-dir ocean_3rdparty/build \
        --expected-matrix matrix.json --json audit.json
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Loaded by path rather than through `lib`, whose __init__ imports the whole
# build system and with it PyYAML. This tool only inspects directories and must
# keep running without it.
_shapes_spec = importlib.util.spec_from_file_location(
    "ocean_artifact_shapes",
    Path(__file__).resolve().parent / "lib" / "artifact_shapes.py",
)
_shapes = importlib.util.module_from_spec(_shapes_spec)
sys.modules[_shapes_spec.name] = _shapes
_shapes_spec.loader.exec_module(_shapes)

framework_binary_name = _shapes.framework_binary_name
is_shared_library_name = _shapes.is_shared_library_name
strip_version = _shapes.strip_version
target_without_link_type = _shapes.target_without_link_type

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


def artifact_kind(
    path: Path, is_windows: bool, relative_to: Optional[Path] = None
) -> Tuple[Optional[str], Optional[str]]:
    """Classify one file, returning (kind, dll_name); (None, None) if not a library.

    `relative_to` is the staging root, used to spot a framework binary from its
    position in the bundle. Without it a framework is invisible: its binary has
    no extension, so a tree holding both `libpng16.a` and
    `libpng16.framework/Versions/A/libpng16` would report a single variant.
    """
    name = path.name
    if is_shared_library_name(name):
        return LibKind.SHARED, None
    suffix = path.suffix
    if suffix == ".lib":
        if is_windows:
            return classify_windows_lib(path)
        return LibKind.STATIC, None
    if suffix == ".a":
        return LibKind.STATIC, None
    if relative_to is not None:
        try:
            inside = path.relative_to(relative_to)
        except ValueError:
            inside = path
        if framework_binary_name(inside) is not None:
            return LibKind.SHARED, None
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
    stem = strip_version(name.casefold())
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
    version: str
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
        kind, dll = artifact_kind(path, is_windows, relative_to=staging)
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


def audit_staging(
    staging: Path, library: str, version: str, target: str, requested: str
) -> Finding:
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
        version=version,
        target=target,
        requested=requested,
        verdict=verdict,
        detail=detail,
        artifacts=artifacts,
        dangling_metadata=dangling,
    )


def discover(build_dir: Path) -> Tuple[List[Tuple[Path, str, str, str]], List[Path]]:
    """Find (staging, library, version, target) for every _install tree.

    The version is part of the key: a static result from one version must not
    complete the pairing for a shared result from another, and stale trees for
    an older version outlive a version bump in the same build directory.

    Only `<build>/<library>/<version>/<target>/_install` counts. A nested
    `_install` -- one a subproject wrote inside its own build tree -- would
    otherwise be read with its parent directory standing in for the target,
    inventing a result for a target that was never built. Anything else found
    is returned separately so it can be reported rather than dropped in
    silence.
    """
    found = []
    unexpected = []
    for staging in sorted(build_dir.rglob("_install")):
        if not staging.is_dir():
            continue
        relative = staging.relative_to(build_dir).parts
        if len(relative) != 4:
            unexpected.append(staging)
            continue
        library, version, target, _ = relative
        found.append((staging, library, version, target))
    return found, unexpected


def audited_scope(findings: List[Finding]) -> Dict[Tuple[str, str, str], set]:
    """Which link types were audited, per (library, version, target-minus-link)."""
    scope: Dict[Tuple[str, str, str], set] = {}
    for finding in findings:
        key = (
            finding.library,
            finding.version,
            target_without_link_type(finding.target),
        )
        scope.setdefault(key, set()).add(finding.requested)
    return scope


def _unpaired(findings: List[Finding]) -> List[Tuple[str, str, str, str]]:
    """Entries audited for one link type only, as (library, version, target, missing)."""
    unpaired = []
    for (library, version, base), kinds in sorted(audited_scope(findings).items()):
        for missing in sorted({LibKind.STATIC, LibKind.SHARED} - kinds):
            unpaired.append((library, version, base, missing))
    return unpaired


def load_expected_matrix(path: Path) -> List[dict]:
    """Read the matrix of results the audit is supposed to see.

    The gate cannot infer what should exist from what does exist: if the
    Windows half of a stack was never built, deriving the requirement from the
    findings makes Windows silently stop being required and the run still
    reports clean. So the expectation is declared, not observed.

    Each entry names one (library, version, target) that must be audited, and
    the link types it must be audited for. Producers that legitimately ship
    only one variant -- mbedtls, directshow and android_native_app_glue are
    static-only in the manifest -- list just that one, rather than being held
    to a build that cannot happen.

        {"expected": [
            {"library": "zlib", "version": "1.3.1",
             "target": "macos_arm64", "link_types": ["static", "shared"]}
        ]}

    Generate it with `build_ocean_3rdparty.py --emit-expected-matrix`.
    """
    try:
        raw = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError) as exc:
        raise RuntimeError(f"cannot read expected matrix {path}: {exc}") from exc

    entries = raw.get("expected") if isinstance(raw, dict) else None
    if not isinstance(entries, list) or not entries:
        raise RuntimeError(f"{path}: expected a non-empty 'expected' list")

    valid = {LibKind.STATIC, LibKind.SHARED}
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            raise RuntimeError(f"{path}: entry {index} is not an object")
        for field_name in ("library", "version", "target"):
            if not isinstance(entry.get(field_name), str) or not entry[field_name]:
                raise RuntimeError(f"{path}: entry {index} lacks a '{field_name}'")
        kinds = entry.get("link_types")
        if not isinstance(kinds, list) or not kinds or set(kinds) - valid:
            raise RuntimeError(
                f"{path}: entry {index} needs a 'link_types' list drawn from "
                f"{sorted(valid)}"
            )
    return entries


def check_expected_matrix(findings: List[Finding], expected: List[dict]) -> List[str]:
    """Expected results that were never audited, worst first."""
    scope = audited_scope(findings)
    gaps = []
    for entry in expected:
        key = (
            entry["library"],
            entry["version"],
            target_without_link_type(entry["target"]),
        )
        audited = scope.get(key, set())
        for kind in sorted(set(entry["link_types"]) - audited):
            gaps.append(f"{entry['library']} {entry['version']} {key[2]} ({kind})")
    return sorted(gaps)


_VERDICT_ORDER = {
    "BROKEN_PAIR": 0,
    "DUAL_PUBLIC": 1,
    "WRONG_ONLY": 2,
    "UNCLASSIFIED": 3,
    "DANGLING_METADATA": 4,
    "REVIEW": 5,
    "OK": 6,
    "NO_LIBRARIES": 7,
}

# The two verdicts that describe a tree needing no action. Everything else
# blocks, REVIEW included: "we could not classify this with confidence" means
# removing the filter would not be byte-identical, so it must not read as a
# clean bill of health.
_CLEAN_VERDICTS = ("OK", "NO_LIBRARIES")


def _print_findings(findings: List[Finding]) -> None:
    """Every non-clean finding, worst first."""
    for f in findings:
        if f.verdict == "OK":
            continue
        print(
            f"[{f.verdict}] {f.library} {f.version} ({f.target}, requested {f.requested})"
        )
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


def _print_producers_needing_attention(findings: List[Finding]) -> List[str]:
    """The libraries with a blocking verdict, and why. Returns their names."""
    needs_fixing = sorted(
        {f.library for f in findings if f.verdict not in _CLEAN_VERDICTS}
    )
    if not needs_fixing:
        print("\nEvery audited tree installs exactly the requested variant.")
        return needs_fixing

    print("\nProducers needing attention:")
    for name in needs_fixing:
        verdicts = sorted(
            {
                f.verdict
                for f in findings
                if f.library == name and f.verdict not in _CLEAN_VERDICTS
            }
        )
        print(f"  - {name} ({', '.join(verdicts)})")
    print("\nA REVIEW entry must be resolved or explicitly allowlisted, not ignored.")
    return needs_fixing


def _print_scope(findings: List[Finding]) -> None:
    """What was actually covered.

    Printed whether or not it is complete: a conclusion drawn from one link
    type is only valid for that link type, and the reader has to be able to
    see which is which.
    """
    print("\nAudited scope:")
    for (library, version, base), kinds in sorted(audited_scope(findings).items()):
        print(f"  {library} {version} {base}: {', '.join(sorted(kinds))}")

    unpaired = _unpaired(findings)
    if unpaired:
        print("\nAudited for one link type only, so the pairing question is")
        print("unanswered for these -- this is expected for a static-only producer")
        print("and a gap for anything else:")
        for library, version, base, missing in unpaired:
            print(f"  - {library} {version} {base}: no {missing} result")


def _print_gate(blocked: bool, matrix_gaps: Optional[List[str]]) -> None:
    """The verdict the exit code carries."""
    if matrix_gaps:
        print("\nExpected results that were never audited:")
        for gap in matrix_gaps:
            print(f"  - {gap}")
    if blocked:
        print(
            "\nFilter-removal gate: BLOCKED. The post-install link-type filter "
            "cannot be removed on macOS/Linux until the lists above are empty."
        )
    else:
        print(
            "\nFilter-removal gate: PASS. Every expected result was audited and "
            "installs exactly the requested variant."
        )


def report(
    findings: List[Finding],
    build_dir: Path,
    matrix_gaps: Optional[List[str]] = None,
    gate_evaluated: bool = False,
) -> bool:
    """Print the audit, worst first. Returns True if anything blocks."""
    findings.sort(key=lambda f: (_VERDICT_ORDER.get(f.verdict, 9), f.library, f.target))
    print(f"Audited {len(findings)} staging trees under {build_dir}\n")

    _print_findings(findings)
    needs_fixing = _print_producers_needing_attention(findings)
    _print_scope(findings)

    if not gate_evaluated:
        print(
            "\nFilter-removal gate: NOT EVALUATED. This run only speaks for the "
            "scope above. Pass --expected-matrix to check it against the results "
            "that are supposed to exist."
        )
        return bool(needs_fixing)

    blocked = bool(needs_fixing or matrix_gaps)
    _print_gate(blocked, matrix_gaps)
    return blocked


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
        "--expected-matrix",
        type=Path,
        default=None,
        help=(
            "JSON listing the results that must exist. Without it the run "
            "reports only on what it found and does not evaluate the gate."
        ),
    )
    args = parser.parse_args()

    if not args.build_dir.is_dir():
        print(f"Error: build directory not found: {args.build_dir}")
        return 1

    trees, unexpected = discover(args.build_dir)
    if unexpected:
        print(
            f"Ignoring {len(unexpected)} '_install' directories that are not at "
            "<library>/<version>/<target>/_install:"
        )
        for path in unexpected[:10]:
            print(f"  {path.relative_to(args.build_dir)}")
        if len(unexpected) > 10:
            print(f"  ... and {len(unexpected) - 10} more")
        print()
    if args.library:
        wanted = set(args.library)
        trees = [t for t in trees if t[1] in wanted]

    if not trees:
        print(f"No _install staging trees under {args.build_dir}.")
        print("Run build_ocean_3rdparty.py first, once per link type.")
        return 1

    expected = None
    if args.expected_matrix:
        try:
            expected = load_expected_matrix(args.expected_matrix)
        except RuntimeError as exc:
            print(f"Error: {exc}")
            return 1

    findings = []
    for staging, library, version, target in trees:
        requested = LibKind.SHARED if "shared" in target else LibKind.STATIC
        findings.append(audit_staging(staging, library, version, target, requested))

    matrix_gaps = check_expected_matrix(findings, expected) if expected else None
    blocking = report(
        findings, args.build_dir, matrix_gaps, gate_evaluated=expected is not None
    )

    if args.json:
        args.json.write_text(
            json.dumps([f.__dict__ for f in findings], indent=2), encoding="utf-8"
        )
        print(f"\nWrote {args.json}")

    return 1 if blocking else 0


if __name__ == "__main__":
    sys.exit(main())
