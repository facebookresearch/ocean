#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

"""
Generates *_reference.props files from *_common.props and application *_build.props
files for Ocean Visual Studio projects.

This script parses props files to extract dependency information from *_use.props imports,
then generates corresponding *_reference.props files with ProjectReference elements.

For libraries:
- Parses *_common.props files
- Generates *_reference.props files
- Optionally updates *_build.props files to import the generated reference files

For applications:
- Parses *_build.props files directly (applications don't have *_common.props)
- Generates *_reference.props files
- Optionally updates *_build.props files to import the generated reference files

The script supports multiple Visual Studio project directories:
- win\\vc143: Windows desktop builds
- android\\vc143: Android builds

Usage:
    python generate_reference_props.py                     # Process all directories
    python generate_reference_props.py --root-path ./win/vc143  # Process specific directory
    python generate_reference_props.py --update-build-props     # Also update *_build.props files

Author: Ocean Build System
"""

import argparse
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple


def get_relative_path(from_path: Path, to_path: Path) -> str:
    """Calculate relative path from one directory to a file."""
    try:
        return os.path.relpath(to_path, from_path)
    except ValueError:
        # On Windows, relpath fails if paths are on different drives
        return str(to_path)


def get_dependencies(props_path: Path) -> List[Dict[str, str]]:
    """Parse a props file to extract dependencies from *_use.props imports."""
    dependencies = []

    if not props_path.exists():
        return dependencies

    content = props_path.read_text(encoding="utf-8")
    directory = props_path.parent

    # Match Import statements for *_use.props files
    pattern = r'<Import\s+Condition="[^"]*"\s+Project="([^"]*_use\.props)"'
    matches = re.findall(pattern, content)

    for use_props_relative_path in matches:
        # Resolve the path to the use.props file
        use_props_full_path = (directory / use_props_relative_path).resolve()

        # The vcxproj is in the same directory as the _use.props, with matching base name
        use_props_directory = use_props_full_path.parent
        use_props_name = use_props_full_path.stem  # filename without extension

        # Remove _use suffix to get base name
        base_name = re.sub(r"_use$", "", use_props_name)

        # Find the corresponding vcxproj
        vcxproj_path = use_props_directory / f"{base_name}.vcxproj"

        if vcxproj_path.exists():
            dependencies.append(
                {
                    "vcxproj_path": vcxproj_path,
                    "use_props_path": use_props_full_path,
                    "relative_use_props_path": use_props_relative_path,
                }
            )

    return dependencies


def get_project_guid(vcxproj_path: Path) -> Optional[str]:
    """Get the project GUID from a vcxproj file."""
    if not vcxproj_path.exists():
        return None

    content = vcxproj_path.read_text(encoding="utf-8")

    match = re.search(r"<ProjectGuid>\{([A-Fa-f0-9\-]+)\}</ProjectGuid>", content)
    if match:
        return match.group(1).upper()

    return None


def generate_reference_props(
    source_props_path: Path,
    output_path: Path,
    base_name: str,
    display_name_prefix: str = "Ocean",
) -> bool:
    """Generate a *_reference.props file."""
    # Get dependencies
    dependencies = get_dependencies(source_props_path)

    if not dependencies:
        print(f"  No dependencies found for {base_name}, skipping...")
        return False

    # Build display name
    display_name = f"{display_name_prefix} {base_name[0].upper()}{base_name[1:]} Project References"

    # Build imported property name
    imported_property_name = f"ocean_{base_name}_reference_imported"

    # Build project references
    project_references = []
    output_directory = output_path.parent

    for dependency in dependencies:
        guid = get_project_guid(dependency["vcxproj_path"])

        if guid is None:
            print(f"  WARNING: Could not find GUID for {dependency['vcxproj_path']}")
            continue

        # Calculate relative path from the output directory to the vcxproj
        relative_path = get_relative_path(output_directory, dependency["vcxproj_path"])

        # Convert to Windows path separators
        relative_path = relative_path.replace("/", "\\")

        project_references.append({"path": relative_path, "guid": guid})

    if not project_references:
        print(f"  No valid project references for {base_name}, skipping...")
        return False

    # Build XML content
    xml_lines = [
        '<?xml version="1.0" encoding="utf-8"?>',
        "<!--",
        "  This file was auto-generated by generate_reference_props.py",
        "  Do not edit manually. Re-run the script to regenerate.",
        "-->",
        '<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">',
        "  <PropertyGroup>",
        f"    <_PropertySheetDisplayName>{display_name}</_PropertySheetDisplayName>",
        "  </PropertyGroup>",
        "",
        "  <PropertyGroup>",
        f"    <{imported_property_name}>True</{imported_property_name}>",
        "  </PropertyGroup>",
        "",
        "  <ItemGroup>",
    ]

    for reference in project_references:
        # Use $(MSBuildThisFileDirectory) to make the path relative to this props file's location,
        # not the importing vcxproj's location. This is critical because MSBuild evaluates
        # ProjectReference paths from the vcxproj that imports the props file, not from
        # the props file's directory.
        xml_lines.extend(
            [
                f'    <ProjectReference Include="$(MSBuildThisFileDirectory){reference["path"]}">',
                f"      <Project>{{{reference['guid']}}}</Project>",
                "      <LinkLibraryDependencies>false</LinkLibraryDependencies>",
                "    </ProjectReference>",
            ]
        )

    xml_lines.extend(["  </ItemGroup>", "</Project>", ""])

    # Write the file
    output_path.write_text("\n".join(xml_lines), encoding="utf-8")
    print(f"  Generated: {output_path}")

    return True


def update_library_build_props(
    build_props_path: Path, reference_props_path: Path, base_name: str
) -> bool:
    """Update a library *_build.props file to import the *_reference.props."""
    if not build_props_path.exists():
        print(f"  WARNING: Build props not found: {build_props_path}")
        return False

    content = build_props_path.read_text(encoding="utf-8")
    reference_props_name = f"{base_name}_reference.props"
    common_props_name = f"{base_name}_common.props"
    imported_property_name = f"ocean_{base_name}_reference_imported"

    # Check if already imported
    if reference_props_name in content:
        print(f"  Already imported in: {build_props_path}")
        return False

    # Build the new import line
    new_import_line = f"    <Import Condition=\"'$({imported_property_name})' != 'True'\" Project=\"{reference_props_name}\"/>"

    # Try to find the *_common.props import line and insert after it
    common_props_pattern = rf'(<Import\s+Condition="[^"]*"\s+Project="[^"]*{re.escape(common_props_name)}"\s*/>)'
    match = re.search(common_props_pattern, content)

    if match:
        # Insert the reference import directly after the common import
        insert_position = match.end()
        new_content = (
            content[:insert_position]
            + "\n"
            + new_import_line
            + content[insert_position:]
        )
        build_props_path.write_text(new_content, encoding="utf-8")
        print(f"  Updated: {build_props_path}")
        return True
    else:
        # Fallback: insert at the end of ImportGroup if common.props not found
        pattern = r'(<ImportGroup\s+Label="Dependencies">[\s\S]*?)(</ImportGroup>)'
        match = re.search(pattern, content)

        if match:
            insert_position = match.start(2)
            new_content = (
                content[:insert_position]
                + new_import_line
                + "\n  "
                + content[insert_position:]
            )
            build_props_path.write_text(new_content, encoding="utf-8")
            print(f"  Updated: {build_props_path} (fallback - appended to ImportGroup)")
            return True
        else:
            print(f"  WARNING: Could not find ImportGroup in: {build_props_path}")
            return False


def update_application_build_props(
    build_props_path: Path, reference_props_path: Path, base_name: str
) -> bool:
    """Update an application *_build.props file to import the *_reference.props."""
    if not build_props_path.exists():
        print(f"  WARNING: Build props not found: {build_props_path}")
        return False

    content = build_props_path.read_text(encoding="utf-8")
    reference_props_name = f"{base_name}_reference.props"
    imported_property_name = f"ocean_{base_name}_reference_imported"

    # Check if already imported
    if reference_props_name in content:
        print(f"  Already imported in: {build_props_path}")
        return False

    # Build the new import line
    new_import_line = f"    <Import Condition=\"'$({imported_property_name})' != 'True'\" Project=\"{reference_props_name}\"/>"

    # Insert at the beginning of the ImportGroup (after the opening tag)
    pattern = r'(<ImportGroup\s+Label="Dependencies">)\r?\n'
    match = re.search(pattern, content)

    if match:
        insert_position = match.end()
        new_content = (
            content[:insert_position]
            + new_import_line
            + "\n"
            + content[insert_position:]
        )
        build_props_path.write_text(new_content, encoding="utf-8")
        print(f"  Updated: {build_props_path}")
        return True
    else:
        print(f"  WARNING: Could not find ImportGroup in: {build_props_path}")
        return False


def is_application_build_props(build_props_path: Path) -> bool:
    """Check if a build.props file is for an application (no corresponding common.props)."""
    directory = build_props_path.parent
    base_name = re.sub(r"_build$", "", build_props_path.stem)
    common_props_path = directory / f"{base_name}_common.props"

    return not common_props_path.exists()


def has_direct_dependencies(build_props_path: Path) -> bool:
    """Check if a build.props file has its own *_use.props imports."""
    if not build_props_path.exists():
        return False

    content = build_props_path.read_text(encoding="utf-8")

    return bool(
        re.search(r'<Import\s+Condition="[^"]*"\s+Project="[^"]*_use\.props"', content)
    )


def process_directory(
    directory_path: Path, update_build_props: bool = False
) -> Tuple[int, int]:
    """Process a single directory."""
    print()
    print(f"Processing directory: {directory_path}")
    print("-" * 48)

    if not directory_path.exists():
        print(f"WARNING: Directory not found: {directory_path}")
        return 0, 0

    generated_count = 0
    updated_count = 0

    # Part 1: Process library *_common.props files
    print()
    print("Processing library *_common.props files...")
    print()

    common_props_files = [
        props_file
        for props_file in directory_path.rglob("*_common.props")
        if props_file.name != "ocean_common.props"
    ]

    print(f"Found {len(common_props_files)} *_common.props files")

    for common_props in common_props_files:
        directory = common_props.parent
        base_name = re.sub(r"_common$", "", common_props.stem)

        print(f"Processing: {common_props.name}")

        # Generate reference props
        reference_props_path = directory / f"{base_name}_reference.props"

        generated = generate_reference_props(
            common_props, reference_props_path, base_name
        )

        if generated:
            generated_count += 1

            # Optionally update build props
            if update_build_props:
                build_props_path = directory / f"{base_name}_build.props"
                updated = update_library_build_props(
                    build_props_path, reference_props_path, base_name
                )

                if updated:
                    updated_count += 1

    # Part 2: Process application *_build.props files
    print()
    print("Processing application *_build.props files...")
    print()

    build_props_files = [
        props_file
        for props_file in directory_path.rglob("*_build.props")
        if is_application_build_props(props_file)
        and has_direct_dependencies(props_file)
    ]

    print(
        f"Found {len(build_props_files)} application *_build.props files with dependencies"
    )

    for build_props in build_props_files:
        directory = build_props.parent
        base_name = re.sub(r"_build$", "", build_props.stem)

        print(f"Processing: {build_props.name}")

        # Generate reference props
        reference_props_path = directory / f"{base_name}_reference.props"

        generated = generate_reference_props(
            build_props,
            reference_props_path,
            base_name,
            display_name_prefix="Application",
        )

        if generated:
            generated_count += 1

            # Optionally update build props
            if update_build_props:
                updated = update_application_build_props(
                    build_props, reference_props_path, base_name
                )

                if updated:
                    updated_count += 1

    return generated_count, updated_count


def main():
    """Main entry point for the script."""
    parser = argparse.ArgumentParser(
        description="Generate *_reference.props files for Ocean Visual Studio projects.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python generate_reference_props.py
      Process all Visual Studio project directories.

  python generate_reference_props.py --root-path ./win/vc143
      Process only the Windows vc143 directory.

  python generate_reference_props.py --update-build-props
      Process all directories and update *_build.props files.
        """,
    )

    parser.add_argument(
        "--root-path",
        type=str,
        default=None,
        help="Process only the specified directory. If not specified, processes all known platform directories.",
    )

    parser.add_argument(
        "--update-build-props",
        action="store_true",
        help="Also update *_build.props files to import the generated *_reference.props files.",
    )

    args = parser.parse_args()

    print("Ocean Visual Studio Project Reference Generator")
    print("=" * 48)
    print()

    total_generated = 0
    total_updated = 0

    # Get the script directory
    script_directory = Path(__file__).parent.resolve()

    if args.root_path is None:
        # No root path specified - process all known directories
        directories = [
            script_directory / "win" / "vc143",
            script_directory / "android" / "vc143",
        ]

        print("Processing all platform directories...")

        for directory_path in directories:
            if directory_path.exists():
                generated, updated = process_directory(
                    directory_path, args.update_build_props
                )
                total_generated += generated
                total_updated += updated
            else:
                print(f"Skipping (not found): {directory_path}")
    else:
        # Process only the specified root path
        root_path = Path(args.root_path).resolve()
        print(f"Root Path: {root_path}")
        total_generated, total_updated = process_directory(
            root_path, args.update_build_props
        )

    print()
    print("=" * 48)
    print("Summary:")
    print(f"  Generated {total_generated} *_reference.props files")

    if args.update_build_props:
        print(f"  Updated {total_updated} *_build.props files")

    print()
    print("Done!")


if __name__ == "__main__":
    main()
