#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Validate dependencies.yaml against the JSON schema.

Usage:
    ./validate_manifest.py                    # Validate default manifest
    ./validate_manifest.py path/to/deps.yaml  # Validate specific file
    ./validate_manifest.py --verbose          # Show detailed output
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    print("Error: PyYAML not installed. Run: pip install PyYAML")
    sys.exit(1)

try:
    from jsonschema import Draft7Validator
except ImportError:
    print("Error: jsonschema not installed. Run: pip install jsonschema")
    sys.exit(1)


def load_schema() -> dict:
    """Load the JSON schema."""
    schema_path = Path(__file__).parent / "schema" / "dependencies.schema.json"
    if not schema_path.exists():
        print(f"Error: Schema not found: {schema_path}")
        sys.exit(1)

    with open(schema_path) as f:
        return json.load(f)


def load_manifest(path: Path) -> dict:
    """Load and parse the YAML manifest."""
    if not path.exists():
        print(f"Error: Manifest not found: {path}")
        sys.exit(1)

    with open(path) as f:
        return yaml.safe_load(f)


def validate_schema(manifest: dict, schema: dict, verbose: bool = False) -> list[str]:
    """Validate manifest against JSON schema."""
    errors = []
    validator = Draft7Validator(schema)

    for error in sorted(validator.iter_errors(manifest), key=lambda e: e.path):
        path = ".".join(str(p) for p in error.path) or "(root)"
        errors.append(f"  {path}: {error.message}")

    return errors


def validate_dependencies(manifest: dict) -> list[str]:
    """Validate that all dependencies reference existing libraries."""
    errors = []
    libraries = manifest.get("libraries", {})

    for lib_name, lib_config in libraries.items():
        deps = lib_config.get("dependencies", [])
        for dep in deps:
            if dep not in libraries:
                errors.append(f"  {lib_name}: depends on unknown library '{dep}'")

    return errors


def detect_cycles(manifest: dict) -> list[str]:
    """Detect circular dependencies."""
    errors = []
    libraries = manifest.get("libraries", {})

    def has_cycle(
        lib: str, visited: set, rec_stack: set, path: list
    ) -> list[str] | None:
        visited.add(lib)
        rec_stack.add(lib)
        path.append(lib)

        deps = libraries.get(lib, {}).get("dependencies", [])
        for dep in deps:
            if dep not in visited:
                result = has_cycle(dep, visited, rec_stack, path)
                if result:
                    return result
            elif dep in rec_stack:
                # Found cycle
                cycle_start = path.index(dep)
                return path[cycle_start:] + [dep]

        path.pop()
        rec_stack.remove(lib)
        return None

    visited = set()
    for lib in libraries:
        if lib not in visited:
            cycle = has_cycle(lib, visited, set(), [])
            if cycle:
                errors.append(f"  Circular dependency: {' -> '.join(cycle)}")
                break

    return errors


def validate_urls(manifest: dict) -> list[str]:
    """Validate that source URLs are properly formatted."""
    errors = []
    libraries = manifest.get("libraries", {})

    for lib_name, lib_config in libraries.items():
        source = lib_config.get("source", {})
        source_type = source.get("type")

        if source_type == "git":
            url = source.get("url", "")
            if not url.startswith(("https://", "git@", "ssh://")):
                errors.append(
                    f"  {lib_name}: git URL should start with https://, git@, or ssh://"
                )
            if not source.get("ref"):
                errors.append(
                    f"  {lib_name}: git source missing 'ref' (tag, branch, or commit)"
                )

    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate dependencies.yaml")
    parser.add_argument(
        "manifest",
        nargs="?",
        default=None,
        help="Path to manifest file (default: dependencies.yaml)",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Show detailed output",
    )
    args = parser.parse_args()

    # Find manifest
    if args.manifest:
        manifest_path = Path(args.manifest)
    else:
        manifest_path = Path(__file__).parent / "dependencies.yaml"

    print(f"Validating: {manifest_path}")

    # Load files
    schema = load_schema()
    manifest = load_manifest(manifest_path)

    all_errors = []

    # Schema validation
    print("\n1. Schema validation...")
    errors = validate_schema(manifest, schema, args.verbose)
    if errors:
        print("   FAILED")
        all_errors.extend(errors)
    else:
        print("   OK")

    # Dependency validation
    print("2. Dependency references...")
    errors = validate_dependencies(manifest)
    if errors:
        print("   FAILED")
        all_errors.extend(errors)
    else:
        print("   OK")

    # Cycle detection
    print("3. Cycle detection...")
    errors = detect_cycles(manifest)
    if errors:
        print("   FAILED")
        all_errors.extend(errors)
    else:
        print("   OK")

    # URL validation
    print("4. URL validation...")
    errors = validate_urls(manifest)
    if errors:
        print("   FAILED")
        all_errors.extend(errors)
    else:
        print("   OK")

    # Summary
    libraries = manifest.get("libraries", {})
    required = [n for n, c in libraries.items() if not c.get("optional")]
    optional = [n for n, c in libraries.items() if c.get("optional")]

    print("\nSummary:")
    print(
        f"  Libraries: {len(libraries)} total ({len(required)} required, {len(optional)} optional)"
    )

    if all_errors:
        print(f"\nErrors found ({len(all_errors)}):")
        for error in all_errors:
            print(error)
        return 1
    else:
        print("\n✓ Manifest is valid!")
        return 0


if __name__ == "__main__":
    sys.exit(main())
