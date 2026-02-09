#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Visualize the dependency graph from dependencies.yaml.

Usage:
    ./visualize_deps.py                    # ASCII list output
    ./visualize_deps.py --format png       # Render PNG image (requires graphviz)
    ./visualize_deps.py --format svg       # Render SVG image
    ./visualize_deps.py --format dot       # GraphViz DOT source
    ./visualize_deps.py --format mermaid   # Mermaid diagram format
    ./visualize_deps.py --format levels    # Show build levels only
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    print("Error: PyYAML not installed. Run: pip install PyYAML")
    sys.exit(1)


def load_manifest(path: Path) -> dict:
    """Load and parse the YAML manifest."""
    if not path.exists():
        print(f"Error: Manifest not found: {path}")
        sys.exit(1)

    with open(path) as f:
        return yaml.safe_load(f)


def get_build_levels(libraries: dict) -> list[list[str]]:
    """Compute build levels using topological sort."""
    # Build reverse dependency map
    dependents = {name: set() for name in libraries}
    in_degree = dict.fromkeys(libraries, 0)

    for name, config in libraries.items():
        for dep in config.get("dependencies", []):
            if dep in libraries:
                dependents[dep].add(name)
                in_degree[name] += 1

    # Kahn's algorithm for topological sort by levels
    levels = []
    remaining = set(libraries.keys())

    while remaining:
        # Find all nodes with no unprocessed dependencies
        level = [n for n in remaining if in_degree[n] == 0]
        if not level:
            print("Error: Circular dependency detected!")
            sys.exit(1)

        levels.append(sorted(level))

        # Remove this level and update in-degrees
        for node in level:
            remaining.remove(node)
            for dependent in dependents[node]:
                in_degree[dependent] -= 1

    return levels


def print_ascii_tree(libraries: dict, show_optional: bool = True) -> None:
    """Print ASCII tree visualization."""
    levels = get_build_levels(libraries)

    print("\nDependency Graph (Build Order)\n")
    print("=" * 60)

    for i, level in enumerate(levels):
        print(f"\nLevel {i}: (can build in parallel)")
        print("-" * 40)
        for lib_name in level:
            config = libraries[lib_name]
            deps = config.get("dependencies", [])
            optional = config.get("optional", False)
            version = config.get("version", "?")

            marker = "[optional]" if optional else ""
            deps_str = f" <- {', '.join(deps)}" if deps else ""

            if show_optional or not optional:
                print(f"  {lib_name} ({version}){deps_str} {marker}")

    print("\n" + "=" * 60)
    print(f"Total: {len(libraries)} libraries in {len(levels)} levels")


def print_dot(libraries: dict) -> None:
    """Print GraphViz DOT format."""
    print("digraph dependencies {")
    print("  rankdir=BT;")
    print("  node [shape=box, style=rounded];")
    print()

    # Define nodes with colors
    for name, config in libraries.items():
        optional = config.get("optional", False)
        color = "lightgray" if optional else "lightblue"
        version = config.get("version", "")
        label = f"{name}\\n{version}"
        print(
            f'  "{name}" [label="{label}", fillcolor="{color}", style="filled,rounded"];'
        )

    print()

    # Define edges
    for name, config in libraries.items():
        for dep in config.get("dependencies", []):
            print(f'  "{dep}" -> "{name}";')

    print("}")


def print_mermaid(libraries: dict) -> None:
    """Print Mermaid diagram format."""
    print("```mermaid")
    print("graph BT")

    # Define nodes
    for name, config in libraries.items():
        optional = config.get("optional", False)
        version = config.get("version", "")
        style = ":::optional" if optional else ""
        print(f"    {name}[{name}<br/>{version}]{style}")

    print()

    # Define edges
    for name, config in libraries.items():
        for dep in config.get("dependencies", []):
            print(f"    {dep} --> {name}")

    print()
    print("    classDef optional fill:#f9f,stroke:#333")
    print("```")


def print_levels(libraries: dict) -> None:
    """Print just the build levels."""
    levels = get_build_levels(libraries)

    print("\nBuild Levels\n")
    for i, level in enumerate(levels):
        libs = ", ".join(level)
        print(f"Level {i}: {libs}")

    print(f"\nTotal: {len(libraries)} libraries, {len(levels)} levels")
    print(f"Maximum parallelism at level 0: {len(levels[0])} libraries")


def render_graphviz(  # noqa: C901
    libraries: dict, output_format: str = "png", output_file: str = None
) -> None:
    """Render graph using graphviz Python library."""
    try:
        import graphviz
    except ImportError:
        print("Error: graphviz not installed. Run: pip install graphviz")
        print("Also ensure Graphviz is installed on your system:")
        print("  macOS: brew install graphviz")
        print("  Ubuntu: apt install graphviz")
        sys.exit(1)

    levels = get_build_levels(libraries)

    # Create directed graph
    dot = graphviz.Digraph(
        comment="Ocean Third-Party Dependencies",
        format=output_format,
        engine="dot",
    )

    # Graph attributes
    dot.attr(rankdir="BT")  # Bottom to top
    dot.attr(splines="ortho")  # Orthogonal edges
    dot.attr(nodesep="0.5")
    dot.attr(ranksep="0.8")

    # Node defaults
    dot.attr("node", shape="box", style="rounded,filled", fontname="Helvetica")

    # Create subgraphs for each level to enforce ranking
    for level_idx, level in enumerate(levels):
        with dot.subgraph() as s:
            s.attr(rank="same")
            for name in level:
                config = libraries[name]
                optional = config.get("optional", False)
                version = config.get("version", "")

                # Node styling
                if optional:
                    color = "#f9f9f9"
                    border = "#999999"
                else:
                    # Color by level
                    colors = ["#a8d5ba", "#b8d4e3", "#f5d5a8", "#d5a8d5"]
                    color = colors[level_idx % len(colors)]
                    border = "#333333"

                label = f"{name}\\n{version}"
                s.node(name, label=label, fillcolor=color, color=border)

    # Add edges
    for name, config in libraries.items():
        for dep in config.get("dependencies", []):
            if dep in libraries:
                dot.edge(dep, name)

    # Determine output filename
    if output_file is None:
        output_file = f"dependencies.{output_format}"

    # Render
    try:
        output_path = dot.render(
            output_file.replace(f".{output_format}", ""), cleanup=True
        )
        print(f"Rendered graph to: {output_path}")

        # Try to open on macOS
        import platform

        if platform.system() == "Darwin":
            import subprocess

            subprocess.run(["open", output_path], check=False)
    except graphviz.ExecutableNotFound:
        print("Error: Graphviz executables not found.")
        print("Install Graphviz:")
        print("  macOS: brew install graphviz")
        print("  Ubuntu: apt install graphviz")
        sys.exit(1)


def main() -> int:
    parser = argparse.ArgumentParser(description="Visualize dependency graph")
    parser.add_argument(
        "manifest",
        nargs="?",
        default=None,
        help="Path to manifest file (default: dependencies.yaml)",
    )
    parser.add_argument(
        "-f",
        "--format",
        choices=["ascii", "png", "svg", "pdf", "dot", "mermaid", "levels"],
        default="ascii",
        help="Output format (default: ascii)",
    )
    parser.add_argument(
        "-o",
        "--output",
        help="Output file path (for png/svg/pdf formats)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Include optional libraries",
    )
    args = parser.parse_args()

    # Find manifest
    if args.manifest:
        manifest_path = Path(args.manifest)
    else:
        manifest_path = Path(__file__).parent / "dependencies.yaml"

    manifest = load_manifest(manifest_path)
    libraries = manifest.get("libraries", {})

    if args.format == "ascii":
        print_ascii_tree(libraries, show_optional=args.all)
    elif args.format in ("png", "svg", "pdf"):
        render_graphviz(libraries, output_format=args.format, output_file=args.output)
    elif args.format == "dot":
        print_dot(libraries)
    elif args.format == "mermaid":
        print_mermaid(libraries)
    elif args.format == "levels":
        print_levels(libraries)

    return 0


if __name__ == "__main__":
    sys.exit(main())
