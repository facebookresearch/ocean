# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# @noautodeps

"""Dependency graph and DAG solver."""

from __future__ import annotations

from collections import defaultdict, deque
from dataclasses import dataclass
from typing import Dict, Iterator, List, Optional, Set

from .manifest import LibraryConfig, Manifest


@dataclass
class DependencyGraph:
    """Directed acyclic graph of library dependencies."""

    # Library name -> list of dependencies
    _deps: Dict[str, List[str]]
    # Library name -> list of libraries that depend on it
    _reverse_deps: Dict[str, List[str]]
    # All library names
    _nodes: Set[str]

    @classmethod
    def from_manifest(
        cls,
        manifest: Manifest,
        libraries: Optional[Dict[str, LibraryConfig]] = None,
    ) -> "DependencyGraph":
        """Build dependency graph from manifest.

        Args:
            manifest: The full manifest
            libraries: Subset of libraries to include (default: all required)
        """
        if libraries is None:
            libraries = manifest.get_required_libraries()

        deps: Dict[str, List[str]] = {}
        reverse_deps: Dict[str, List[str]] = defaultdict(list)
        nodes: Set[str] = set()

        for name, lib in libraries.items():
            nodes.add(name)
            # Only include dependencies that are in our library set
            lib_deps = [d for d in lib.dependencies if d in libraries]
            deps[name] = lib_deps

            for dep in lib_deps:
                reverse_deps[dep].append(name)

        return cls(
            _deps=deps,
            _reverse_deps=dict(reverse_deps),
            _nodes=nodes,
        )

    def get_dependencies(self, library: str) -> List[str]:
        """Get direct dependencies of a library."""
        return self._deps.get(library, [])

    def get_dependents(self, library: str) -> List[str]:
        """Get libraries that depend on this library."""
        return self._reverse_deps.get(library, [])

    def get_all_dependencies(self, library: str) -> Set[str]:
        """Get all transitive dependencies of a library."""
        result: Set[str] = set()
        stack = list(self._deps.get(library, []))

        while stack:
            dep = stack.pop()
            if dep not in result:
                result.add(dep)
                stack.extend(self._deps.get(dep, []))

        return result

    def topological_sort(self) -> List[str]:
        """Return libraries in topological order (dependencies first).

        Uses Kahn's algorithm for stability and clarity.
        """
        # Calculate in-degrees
        in_degree: Dict[str, int] = dict.fromkeys(self._nodes, 0)
        for node in self._nodes:
            for _dep in self._deps.get(node, []):
                # dep must be built before node, so node has an incoming edge from dep
                pass
            # Actually, we need to count how many things depend on each node
            # No wait - in_degree should count dependencies, not dependents

        # Recalculate: in_degree[x] = number of dependencies x has
        in_degree = {node: len(self._deps.get(node, [])) for node in self._nodes}

        # Start with nodes that have no dependencies
        queue = deque([node for node, degree in in_degree.items() if degree == 0])
        result: List[str] = []

        while queue:
            node = queue.popleft()
            result.append(node)

            # For each node that depends on this one, reduce its in-degree
            for dependent in self._reverse_deps.get(node, []):
                in_degree[dependent] -= 1
                if in_degree[dependent] == 0:
                    queue.append(dependent)

        if len(result) != len(self._nodes):
            missing = self._nodes - set(result)
            raise ValueError(
                f"Cycle detected in dependency graph. Stuck nodes: {missing}"
            )

        return result

    def get_build_levels(self) -> List[List[str]]:
        """Group libraries into dependency levels for parallel building.

        Level 0: Libraries with no dependencies (can all build in parallel)
        Level 1: Libraries depending only on Level 0 (can all build in parallel after L0)
        Level 2: Libraries depending on Level 0 or 1
        ...

        Returns:
            List of levels, where each level is a list of library names.
        """
        # Calculate the level for each library
        levels: Dict[str, int] = {}

        def get_level(name: str) -> int:
            if name in levels:
                return levels[name]

            deps = self._deps.get(name, [])
            if not deps:
                levels[name] = 0
            else:
                # Level is 1 + max level of dependencies
                levels[name] = 1 + max(get_level(dep) for dep in deps)

            return levels[name]

        # Calculate levels for all nodes
        for name in self._nodes:
            get_level(name)

        # Group by level
        max_level = max(levels.values()) if levels else 0
        result: List[List[str]] = [[] for _ in range(max_level + 1)]

        for name, level in levels.items():
            result[level].append(name)

        # Sort each level for deterministic output
        for level in result:
            level.sort()

        return result

    def get_leaf_nodes(self) -> List[str]:
        """Get libraries with no dependencies (DAG leaves)."""
        return sorted([name for name in self._nodes if not self._deps.get(name)])

    def get_root_nodes(self) -> List[str]:
        """Get libraries that nothing depends on (DAG roots)."""
        return sorted(
            [name for name in self._nodes if not self._reverse_deps.get(name)]
        )

    def __len__(self) -> int:
        return len(self._nodes)

    def __iter__(self) -> Iterator[str]:
        return iter(self._nodes)

    def __contains__(self, item: str) -> bool:
        return item in self._nodes


def print_build_plan(
    graph: DependencyGraph,
    targets: List[str],
    max_parallel: int,
    libraries: Optional[Dict] = None,
) -> None:
    """Print a dry-run build plan.

    Args:
        graph: Dependency graph
        targets: List of target strings (e.g., 'macos_arm64_static')
        max_parallel: Maximum parallel jobs
        libraries: Optional dict of LibraryConfig to filter by platform support
    """
    levels = graph.get_build_levels()

    # Helper to get OS from target string (e.g., 'macos_arm64_static' -> 'macos')
    def get_os(target: str) -> str:
        return target.split("_")[0]

    # Calculate jobs accounting for platform support
    if libraries:
        total_jobs = sum(
            1
            for level in levels
            for lib_name in level
            for target in targets
            if lib_name in libraries
            and libraries[lib_name].supports_platform(get_os(target))
        )
    else:
        total_jobs = sum(len(level) * len(targets) for level in levels)

    total_libs = len(graph)
    total_targets = len(targets)

    print("Build Plan:")
    print(f"  Libraries: {total_libs}")
    print(f"  Targets: {total_targets} ({', '.join(targets)})")
    print(f"  Total jobs: {total_jobs}")
    print(f"  Max parallel: {max_parallel}")
    print()

    for level_idx, level_libs in enumerate(levels):
        if libraries:
            # Count only jobs where library supports target platform
            jobs_in_level = sum(
                1
                for lib_name in level_libs
                for target in targets
                if lib_name in libraries
                and libraries[lib_name].supports_platform(get_os(target))
            )
            # Get unique targets per library
            libs_with_targets = []
            for lib_name in level_libs:
                if lib_name in libraries:
                    lib_targets = [
                        t
                        for t in targets
                        if libraries[lib_name].supports_platform(get_os(t))
                    ]
                    if lib_targets:
                        libs_with_targets.append(lib_name)
            if not libs_with_targets:
                continue
            print(f"Level {level_idx} ({jobs_in_level} jobs):")
            print(f"  {', '.join(libs_with_targets)}")
        else:
            jobs_in_level = len(level_libs) * total_targets
            print(f"Level {level_idx} ({jobs_in_level} jobs):")
            print(f"  {', '.join(level_libs)}")
        print(f"  × {', '.join(targets)}")
        print()
