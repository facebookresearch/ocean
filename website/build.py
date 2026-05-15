#!/usr/bin/env python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

"""
Build script for the Ocean documentation website.

Converts Markdown docs to a static HTML site. Replaces Docusaurus with
a zero-npm-dependency build pipeline.

Requirements:
    pip install markdown

Usage:
    python build.py                     # Build the site
    python build.py --serve             # Build and start local server
    python build.py --skip-fetch        # Skip fetching remote build guides
    python build.py --with-doxygen      # Run Doxygen after build
"""

from __future__ import annotations

import argparse
import html as html_module
import http.server
import json
import logging
import os
import re
import shutil
import subprocess
import sys
import textwrap
import urllib.request
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path

# Use vendored markdown package from third_party/ if available,
# otherwise fall back to system-installed package.
sys.path.insert(0, str(Path(__file__).parent / "third_party"))
try:
    import markdown
except ImportError:
    print("Error: 'markdown' package not found in third_party/ or system.")
    print("Install with: pip install markdown")
    sys.exit(1)

logger: logging.Logger = logging.getLogger(__name__)

# ============================================================================
# Configuration
# ============================================================================

# When built by StaticDocs (FB_INTERNAL=1), use relative paths so that
# the sanity check and favicon resolution work correctly.
# For GitHub Pages, use the absolute /ocean/ prefix.
BASE_URL = "" if os.environ.get("FB_INTERNAL") == "1" else "/ocean/"
SITE_TITLE = "Ocean"
DOCS_DIR = "docs"
STATIC_DIR = "static"
TEMPLATES_DIR = "templates"
ASSETS_DIR = "assets"
BUILD_DIR = "build"

GITHUB_RAW = "https://raw.githubusercontent.com/facebookresearch/ocean/main/"

REMOTE_DOCS = [
    ("doc/building_for_android.md", "Building for Android", 2),
    ("doc/building_for_ios.md", "Building for iOS", 3),
    ("doc/building_for_linux.md", "Building for Linux", 4),
    ("doc/building_for_macos.md", "Building for macOS", 5),
    ("doc/building_for_meta_quest.md", "Building for Meta Quest", 6),
    ("doc/building_for_windows.md", "Building for Windows", 7),
    ("doc/building_with_visual_studio.md", "Building with Visual Studio", 8),
]


def _get_script_dir() -> Path:
    return Path(__file__).parent.resolve()


# ============================================================================
# Data Structures
# ============================================================================


@dataclass
class SidebarItem:
    """A node in the sidebar tree."""

    title: str
    position: int
    url: str = ""  # Empty for non-linked categories
    children: list[SidebarItem] = field(default_factory=list)
    is_category: bool = False


# ============================================================================
# Frontmatter Parser
# ============================================================================


def parse_frontmatter(content: str) -> tuple[dict[str, str | int], str]:
    """Parse YAML frontmatter from markdown content.

    Returns (metadata_dict, body_without_frontmatter).
    """
    match = re.match(r"^---\s*\n(.*?)\n---\s*\n", content, re.DOTALL)
    if not match:
        return {}, content

    meta = {}
    for line in match.group(1).strip().split("\n"):
        line = line.strip()
        if ":" in line:
            key, _, value = line.partition(":")
            value = value.strip().strip("\"'")
            if key.strip() == "sidebar_position":
                try:
                    value = int(value)
                except ValueError:
                    value = 999
            meta[key.strip()] = value

    body = content[match.end() :]
    return meta, body


# ============================================================================
# JSX Preprocessor
# ============================================================================


def preprocess_jsx(content: str, md_converter: markdown.Markdown) -> str:
    """Apply all JSX-to-HTML transforms before markdown conversion."""
    # Strip zero-width spaces that interfere with markdown parsing
    content = content.replace("\u200b", "")
    content = _strip_imports(content)
    content = _convert_require_images(content)
    content = _convert_classname(content)
    content = _convert_jsx_styles(content)
    content = _convert_admonitions(content)
    content = _convert_tabs(content, md_converter)
    content = _fix_indented_lists(content)
    content = _normalize_list_nesting(content)
    return content


def _fix_indented_lists(content: str) -> str:
    """Fix lists indented 4+ spaces that markdown treats as code blocks.

    Some source docs indent bullet/numbered lists under a paragraph,
    which standard markdown interprets as an indented code block. MDX/Docusaurus
    was lenient about this. We detect contiguous blocks of 4-space-indented lines
    that start with list markers ('- ' or '1. ') and dedent them.
    """
    lines = content.split("\n")
    result = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.lstrip()
        indent = len(line) - len(stripped)
        # Detect a line indented 4+ spaces that starts with a list marker
        is_list_start = indent >= 4 and (
            stripped.startswith("- ")
            or stripped.startswith("* ")
            or re.match(r"\d+\.\s", stripped)
        )
        if is_list_start:
            # Dedent this block: remove exactly 4 leading spaces from lines
            # that are list items or list continuations (indented text under
            # a list item). Stop when we hit a non-indented, non-blank line.
            while i < len(lines):
                ln = lines[i]
                if ln.strip() == "":
                    result.append(ln)
                elif ln.startswith("    "):
                    dedented = ln[4:]
                    # Only dedent if this looks like list content (list item
                    # or continuation text), not arbitrary indented code
                    stripped_d = dedented.lstrip()
                    # A dedented line is list content if it is:
                    # - A list marker (bullet or numbered)
                    # - Indented continuation (2+ spaces, under a sub-item)
                    # - Unindented continuation text (wrapping from prev line)
                    # Lines with exactly 1 space of indent after dedenting
                    # are ambiguous and treated as non-list to avoid
                    # corrupting indented code blocks.
                    is_list_content = (
                        stripped_d.startswith("- ")
                        or stripped_d.startswith("* ")
                        or bool(re.match(r"\d+\.\s", stripped_d))
                        or dedented.startswith("  ")
                        or not dedented[0].isspace()
                    )
                    if is_list_content:
                        result.append(dedented)
                    else:
                        # Not list content — stop dedenting
                        break
                else:
                    break
                i += 1
        else:
            result.append(line)
            i += 1
    return "\n".join(result)


def _normalize_list_nesting(content: str) -> str:
    """Normalize non-standard list nesting to 4-space nesting.

    Standard markdown requires 4 spaces per nesting level, but
    Docusaurus/MDX accepted 2 or 3-space nesting. Only adjust indentation
    for bullets that are genuinely nested (i.e., a parent list item at
    lower indentation exists nearby above them).
    """
    lines = content.split("\n")
    result = []
    in_list = False
    base_indent = 0

    for line in lines:
        stripped = line.lstrip()
        indent = len(line) - len(stripped)
        is_list_item = bool(re.match(r"^( *)(- |\* |\d+\. )", line))

        if is_list_item:
            if not in_list:
                in_list = True
                base_indent = indent
                result.append(line)
            elif indent <= base_indent:
                base_indent = min(base_indent, indent)
                result.append(line)
            else:
                # Deeper indent — this is a nested item
                depth = indent - base_indent
                if depth % 4 != 0:
                    new_depth = ((depth // 4) + 1) * 4
                    result.append(" " * (base_indent + new_depth) + stripped)
                else:
                    result.append(line)
        elif line.strip() == "":
            # Blank lines don't end list context
            result.append(line)
        elif in_list and indent > base_indent:
            # Indented non-list line (continuation text under a list item)
            # — keep list context alive
            result.append(line)
        else:
            in_list = False
            result.append(line)

    return "\n".join(result)


def _strip_imports(content: str) -> str:
    """Remove JSX import statements."""
    return re.sub(r"^import\s+.*?;\s*$", "", content, flags=re.MULTILINE)


def _convert_require_images(content: str) -> str:
    """Convert {require('@site/static/img/...').default} to plain paths."""
    return re.sub(
        r"""\{require\(['"](.*?)['"]\)\.default\}""",
        lambda m: '"{}"'.format(m.group(1).replace("@site/static/", BASE_URL)),
        content,
    )


def _convert_classname(content: str) -> str:
    """Convert className= to class=."""
    return re.sub(r"\bclassName=", "class=", content)


def _convert_jsx_styles(content: str) -> str:
    """Convert JSX style={{...}} to HTML style='...'.

    Handles single-quoted, double-quoted, and bare numeric values.
    Limitations: does not handle nested braces, template literals,
    or expressions (e.g., style={{width: someVar}}). These patterns
    do not appear in the current Ocean docs.
    """

    def _style_to_css(match):
        inner = match.group(1)
        # Convert camelCase to kebab-case
        inner = re.sub(
            r"([a-z])([A-Z])", lambda m: m.group(1) + "-" + m.group(2).lower(), inner
        )
        # Extract key-value pairs: handles 'quoted', "quoted", and numeric values
        pairs = re.findall(
            r"""([\w-]+)\s*:\s*(?:'([^']*)'|"([^"]*)"|(\d+(?:\.\d+)?))""",
            inner,
        )
        css_parts = []
        for key, sq, dq, num in pairs:
            val = sq or dq or (num + "px" if num else "")
            if val:
                css_parts.append(f"{key}: {val}")
        css = "; ".join(css_parts)
        return f'style="{css}"'

    return re.sub(r"style=\{\{(.*?)\}\}", _style_to_css, content)


def _convert_admonitions(content: str) -> str:
    """Convert :::tip / :::note / etc. admonitions to HTML."""

    def _admonition_replace(match):
        kind = match.group(1)
        body = match.group(2).strip()
        label = kind.capitalize()
        return (
            f'<div class="admonition admonition--{kind}">\n'
            f'<div class="admonition__title">{label}</div>\n\n'
            f"{body}\n\n"
            f"</div>"
        )

    return re.sub(
        r"^:::(tip|note|warning|danger|info|caution)\s*\n(.*?)\n^:::\s*$",
        _admonition_replace,
        content,
        flags=re.MULTILINE | re.DOTALL,
    )


def _convert_tabs(content: str, md_converter: markdown.Markdown) -> str:
    """Convert <Tabs>/<TabItem> JSX to HTML tab structure.

    The tab content is converted from markdown to HTML inline,
    then the entire <Tabs> block is replaced with raw HTML.
    """
    result = []
    pos = 0

    while True:
        tabs_start = content.find("<Tabs", pos)
        if tabs_start == -1:
            result.append(content[pos:])
            break

        # Add content before this <Tabs> block
        result.append(content[pos:tabs_start])

        # Find the closing </Tabs>
        tabs_end = content.find("</Tabs>", tabs_start)
        if tabs_end == -1:
            result.append(content[tabs_start:])
            break

        tabs_block = content[tabs_start : tabs_end + len("</Tabs>")]

        # Extract groupId
        group_match = re.search(r'groupId="([^"]*)"', tabs_block)
        group_id = group_match.group(1) if group_match else ""

        # Extract TabItems
        tab_items = _extract_tab_items(tabs_block)

        # Build HTML
        html_parts = [f'<div class="tabs" data-group="{group_id}">']
        html_parts.append('<div class="tabs__nav">')

        for i, item in enumerate(tab_items):
            active = (
                " tabs__tab--active"
                if item["default"]
                or (i == 0 and not any(t["default"] for t in tab_items))
                else ""
            )
            html_parts.append(
                f'<button class="tabs__tab{active}" data-tab="{item["value"]}">'
                f"{item['label']}</button>"
            )

        html_parts.append("</div>")

        for i, item in enumerate(tab_items):
            active = (
                " tabs__panel--active"
                if item["default"]
                or (i == 0 and not any(t["default"] for t in tab_items))
                else ""
            )
            # Convert tab content markdown to HTML
            tab_html = convert_markdown(item["content"], md_converter)
            html_parts.append(
                f'<div class="tabs__panel{active}" data-tab="{item["value"]}">'
                f"{tab_html}</div>"
            )

        html_parts.append("</div>")
        result.append("\n".join(html_parts))

        pos = tabs_end + len("</Tabs>")

    return "".join(result)


def _extract_tab_items(tabs_block: str) -> list[dict[str, str | bool]]:
    """Extract TabItem value, label, default, and content from a <Tabs> block."""
    items = []
    pos = 0

    while True:
        item_start = tabs_block.find("<TabItem", pos)
        if item_start == -1:
            break

        # Find the end of the opening tag
        tag_end = tabs_block.find(">", item_start)
        if tag_end == -1:
            break

        opening_tag = tabs_block[item_start : tag_end + 1]

        # Parse attributes
        value = re.search(r'value="([^"]*)"', opening_tag)
        label = re.search(r'label="([^"]*)"', opening_tag)
        is_default = "default" in opening_tag.split(">")[0]

        # Find closing </TabItem>
        close_tag = "</TabItem>"
        item_end = tabs_block.find(close_tag, tag_end)
        if item_end == -1:
            break

        content = tabs_block[tag_end + 1 : item_end]
        # Dedent the content
        content = textwrap.dedent(content).strip()

        items.append(
            {
                "value": value.group(1) if value else "",
                "label": label.group(1) if label else "",
                "default": is_default,
                "content": content,
            }
        )

        pos = item_end + len(close_tag)

    return items


# ============================================================================
# Markdown Converter
# ============================================================================


def create_markdown_converter() -> markdown.Markdown:
    """Create a markdown converter with required extensions."""
    return markdown.Markdown(
        extensions=[
            "markdown.extensions.tables",
            "markdown.extensions.fenced_code",
            "markdown.extensions.toc",
            "markdown.extensions.md_in_html",
            "markdown.extensions.attr_list",
        ],
        extension_configs={
            "markdown.extensions.toc": {"permalink": False},
        },
    )


def convert_markdown(text: str, md_converter: markdown.Markdown | None = None) -> str:
    """Convert markdown text to HTML."""
    if md_converter is None:
        md_converter = create_markdown_converter()
    else:
        md_converter.reset()
    return md_converter.convert(text)


# ============================================================================
# Internal Link Resolver
# ============================================================================


def resolve_doc_links(content: str, current_doc_path: Path) -> str:
    """Convert relative .md links to final URL paths."""

    def _replace_link(match):
        text = match.group(1)
        href = match.group(2)

        # Skip external, anchor, and image links
        if href.startswith(("http://", "https://", "#", "/ocean/")):
            return match.group(0)

        if not href.endswith(".md"):
            return match.group(0)

        # Resolve relative path
        current_dir = current_doc_path.parent
        resolved = (current_dir / href).resolve()

        try:
            rel = resolved.relative_to(_get_script_dir() / DOCS_DIR)
        except ValueError:
            return match.group(0)

        # Convert to URL path
        url = doc_path_to_url(rel)
        return f"[{text}]({url})"

    return re.sub(r"\[([^\]]*)\]\(([^)]+)\)", _replace_link, content)


def doc_path_to_url(rel_path: Path) -> str:
    """Convert a doc-relative path to a URL path.

    e.g. building/building.md -> /ocean/docs/building/
         building/building_for_android.md -> /ocean/docs/building/building_for_android/
         introduction.md -> /ocean/docs/introduction/
    """
    stem = rel_path.stem
    parent = rel_path.parent

    # Same-name file as directory becomes the directory index
    if parent.name == stem:
        url = f"{BASE_URL}docs/{parent}/"
    elif str(parent) == ".":
        url = f"{BASE_URL}docs/{stem}/"
    else:
        url = f"{BASE_URL}docs/{parent}/{stem}/"

    return url


# ============================================================================
# Sidebar Builder
# ============================================================================


def build_sidebar(docs_dir: Path) -> tuple[list[SidebarItem], list[tuple[str, str]]]:
    """Build sidebar tree from docs directory.

    Returns (sidebar_tree, flat_page_list) where flat_page_list
    is ordered for prev/next navigation.
    """
    tree = _scan_directory(docs_dir, docs_dir)
    flat = _flatten_tree(tree)
    return tree, flat


def _scan_directory(dir_path: Path, docs_root: Path) -> list[SidebarItem]:
    """Recursively scan a directory and build sidebar items."""
    items = []

    # Check for _category_.json
    category_file = dir_path / "_category_.json"
    category_meta = {}
    if category_file.exists():
        with open(category_file) as f:
            category_meta = json.load(f)

    # Scan files
    for entry in sorted(dir_path.iterdir()):
        if entry.name.startswith(".") or entry.name.startswith("_"):
            continue
        if entry.name == "fb":  # Skip meta-only directory
            continue

        if entry.is_dir():
            children = _scan_directory(entry, docs_root)

            cat_file = entry / "_category_.json"
            cat_meta = {}
            if cat_file.exists():
                with open(cat_file) as f:
                    cat_meta = json.load(f)

            # Check if category has a linked doc (same name as directory)
            cat_url = ""
            linked_doc = entry / f"{entry.name}.md"
            if linked_doc.exists():
                rel = linked_doc.relative_to(docs_root)
                cat_url = doc_path_to_url(rel)

            # Also check for category link in _category_.json
            if "link" in cat_meta:
                link = cat_meta["link"]
                if link.get("type") == "doc" and "id" in link:
                    link_path = docs_root / (link["id"] + ".md")
                    if link_path.exists():
                        rel = link_path.relative_to(docs_root)
                        cat_url = doc_path_to_url(rel)

            # If category has a linked doc but no children, render as a
            # simple link rather than an empty collapsible section
            if not children and cat_url:
                items.append(
                    SidebarItem(
                        title=cat_meta.get(
                            "label", entry.name.replace("_", " ").title()
                        ),
                        position=cat_meta.get("position", 999),
                        url=cat_url,
                    )
                )
            elif children or cat_url:
                items.append(
                    SidebarItem(
                        title=cat_meta.get(
                            "label", entry.name.replace("_", " ").title()
                        ),
                        position=cat_meta.get("position", 999),
                        url=cat_url,
                        children=children,
                        is_category=True,
                    )
                )

        elif entry.suffix == ".md":
            with open(entry) as f:
                content = f.read()
            meta, _ = parse_frontmatter(content)

            rel = entry.relative_to(docs_root)
            url = doc_path_to_url(rel)

            # Skip category index docs from the child list if they're the category link
            parent_name = entry.parent.name
            if entry.stem == parent_name:
                # This doc is the category link; it will appear as the
                # category header link, not as a child
                continue

            items.append(
                SidebarItem(
                    title=meta.get("title", entry.stem.replace("_", " ").title()),
                    position=meta.get("sidebar_position", 999),
                    url=url,
                )
            )

        elif entry.suffix == ".css":
            # Skip CSS files
            continue

    items.sort(key=lambda x: (x.position, x.title))
    return items


def _flatten_tree(items: list[SidebarItem]) -> list[tuple[str, str]]:
    """Flatten sidebar tree into ordered list of (title, url) for prev/next."""
    result = []
    for item in items:
        if item.url:
            result.append((item.title, item.url))
        if item.children:
            result.extend(_flatten_tree(item.children))
    return result


def _render_category(html: list[str], item: SidebarItem, current_url: str) -> None:
    """Render a single category item (with children) into the html list."""
    is_expanded = _category_contains_url(item, current_url)
    expanded_class = " sidebar__category--expanded" if is_expanded else ""

    html.append(f'<li class="sidebar__category{expanded_class}">')

    if item.url:
        active = " sidebar__link--active" if item.url == current_url else ""
        html.append(
            f'<div class="sidebar__category-header">'
            f'<span class="sidebar__chevron"></span>'
            f'<a class="sidebar__link{active}" href="{item.url}">{html_module.escape(item.title)}</a>'
            f"</div>"
        )
    else:
        html.append(
            f'<div class="sidebar__category-header">'
            f'<span class="sidebar__chevron"></span>'
            f"<span>{html_module.escape(item.title)}</span>"
            f"</div>"
        )

    html.append('<ul class="sidebar__category-children">')
    for child in item.children:
        if child.is_category:
            _render_category(html, child, current_url)
        else:
            active = " sidebar__link--active" if child.url == current_url else ""
            html.append(
                f'<li><a class="sidebar__link{active}" href="{child.url}">'
                f"{html_module.escape(child.title)}</a></li>"
            )
    html.append("</ul>")
    html.append("</li>")


def render_sidebar_html(items: list[SidebarItem], current_url: str) -> str:
    """Render sidebar tree as HTML."""
    html: list[str] = ["<ul>"]
    for item in items:
        if item.is_category:
            _render_category(html, item, current_url)
        else:
            active = " sidebar__link--active" if item.url == current_url else ""
            html.append(
                f'<li><a class="sidebar__link{active}" href="{item.url}">'
                f"{html_module.escape(item.title)}</a></li>"
            )
    html.append("</ul>")
    return "\n".join(html)


def _category_contains_url(item: SidebarItem, url: str) -> bool:
    """Check if a category or its children contain the given URL."""
    if item.url == url:
        return True
    for child in item.children:
        if child.url == url:
            return True
        if child.is_category and _category_contains_url(child, url):
            return True
    return False


# ============================================================================
# Pagination
# ============================================================================


def render_pagination(flat_pages: list[tuple[str, str]], current_url: str) -> str:
    """Generate prev/next navigation HTML."""
    idx = None
    for i, (title, url) in enumerate(flat_pages):
        if url == current_url:
            idx = i
            break

    if idx is None:
        return ""

    html = ['<nav class="pagination">']

    if idx > 0:
        prev_title, prev_url = flat_pages[idx - 1]
        html.append(
            f'<a class="pagination__link pagination__link--prev" href="{prev_url}">'
            f'<span class="pagination__label">Previous</span>'
            f'<span class="pagination__title">{prev_title}</span>'
            f"</a>"
        )
    else:
        html.append("<div></div>")

    if idx < len(flat_pages) - 1:
        next_title, next_url = flat_pages[idx + 1]
        html.append(
            f'<a class="pagination__link pagination__link--next" href="{next_url}">'
            f'<span class="pagination__label">Next</span>'
            f'<span class="pagination__title">{next_title}</span>'
            f"</a>"
        )

    html.append("</nav>")
    return "\n".join(html)


# ============================================================================
# Remote Content Fetcher
# ============================================================================


def fetch_remote_docs(docs_dir: Path) -> list[tuple[str, bool]]:
    """Fetch build guides from GitHub.

    Returns list of (filename, success) tuples.
    """
    results = []
    building_dir = docs_dir / "building"
    building_dir.mkdir(parents=True, exist_ok=True)

    for remote_path, title, position in REMOTE_DOCS:
        filename = remote_path.replace("doc/", "")
        dest = building_dir / filename
        url = GITHUB_RAW + remote_path

        print(f"  Fetching {remote_path}...", end=" ")

        try:
            req = urllib.request.Request(url)
            with urllib.request.urlopen(req, timeout=30) as resp:
                content = resp.read().decode("utf-8")

            # Apply content transformations (matching Docusaurus plugin)
            content = _transform_build_guide(content, remote_path, title, position)

            with open(dest, "w") as f:
                f.write(content)

            print("OK")
            results.append((filename, True))

        except Exception:
            logger.exception(f"Failed to fetch {remote_path}")
            print("FAILED")
            results.append((filename, False))

    return results


def _transform_build_guide(
    content: str, filename: str, title: str, position: int
) -> str:
    """Apply content transforms to fetched build guide."""
    # Convert relative image paths to website static assets
    content = re.sub(
        r"!\[([^\]]*)\]\(images/([^)]+)\)",
        rf"![\1]({BASE_URL}img/building/\2)",
        content,
    )

    # Convert remaining relative links to GitHub blob URLs
    content = re.sub(
        r"(?<!!)\]\((?!https?://|#|/ocean/img/)([^)]+)\)",
        r"](https://github.com/facebookresearch/ocean/blob/main/doc/\1)",
        content,
    )

    # Remove the first H1 heading
    content = re.sub(r"^# .+\n\n?", "", content, count=1)

    # Add frontmatter
    github_url = f"https://github.com/facebookresearch/ocean/blob/main/{filename}"
    tip = (
        f"\n\n:::tip\n"
        f"This documentation is also available on [GitHub]({github_url}).\n"
        f":::\n"
    )

    content = (
        f"---\n"
        f"title: {title}\n"
        f"sidebar_position: {position}\n"
        f"description: {title}\n"
        f"---\n\n"
        f"{content}{tip}"
    )

    return content


# ============================================================================
# Building Images Copier
# ============================================================================


def copy_building_images(build_dir: Path):
    """Copy images from ../doc/images/ to build/img/building/."""
    src_dir = _get_script_dir().parent / "doc" / "images"
    dest_dir = build_dir / "img" / "building"

    if not src_dir.exists():
        print(f"  Warning: {src_dir} not found, skipping building images")
        return

    dest_dir.mkdir(parents=True, exist_ok=True)
    count = 0
    for f in src_dir.iterdir():
        if f.is_file():
            shutil.copy2(f, dest_dir / f.name)
            count += 1
    print(f"  Copied {count} building images")


# ============================================================================
# Page Builder
# ============================================================================


def build_page(
    doc_path: Path,
    docs_root: Path,
    template: str,
    sidebar_tree: list[SidebarItem],
    flat_pages: list[tuple[str, str]],
    md_converter: markdown.Markdown,
) -> tuple[Path, bool]:
    """Build a single doc page. Returns (output_path, success)."""
    with open(doc_path) as f:
        raw = f.read()

    meta, body = parse_frontmatter(raw)
    title = meta.get("title", doc_path.stem.replace("_", " ").title())
    description = meta.get("description", title)

    rel_path = doc_path.relative_to(docs_root)
    url = doc_path_to_url(rel_path)

    # Resolve internal doc links
    body = resolve_doc_links(body, doc_path.relative_to(_get_script_dir()))

    # Preprocess JSX
    body = preprocess_jsx(body, md_converter)

    # Convert markdown to HTML
    content_html = convert_markdown(body, md_converter)

    # Build sidebar for this page
    sidebar_html = render_sidebar_html(sidebar_tree, url)

    # Build pagination
    pagination_html = render_pagination(flat_pages, url)

    # Render template using simple string replacement to avoid
    # conflicts with curly braces in HTML/JS content.
    #
    # Security notes:
    # - title/description are HTML-escaped (user-provided frontmatter text
    #   inserted into <title>, <meta>, and <h1> contexts).
    # - sidebar_html, content_html, pagination_html are intentionally
    #   unescaped HTML generated by our own render functions
    #   (render_sidebar_html, convert_markdown, render_pagination).
    #   These functions produce trusted HTML from controlled inputs:
    #   sidebar titles are escaped via html_module.escape() in
    #   _render_category/render_sidebar_html, content comes from
    #   markdown conversion of checked-in .md files, and pagination
    #   uses the same escaped sidebar titles. External content (fetched
    #   build guides) is also from a controlled source
    #   (raw.githubusercontent.com/facebookresearch/ocean).
    safe_title = html_module.escape(str(title))
    safe_description = html_module.escape(str(description))
    year = str(datetime.now().year)
    page_html = (
        template.replace("{title}", safe_title)
        .replace("{description}", safe_description)
        .replace("{base_url}", BASE_URL)
        .replace("{sidebar_html}", sidebar_html)
        .replace("{content_html}", content_html)
        .replace("{pagination_html}", pagination_html)
        .replace("{year}", year)
    )

    # Determine output path
    stem = rel_path.stem
    parent = rel_path.parent

    if parent.name == stem:
        out_path = Path(BUILD_DIR) / "docs" / parent / "index.html"
    elif str(parent) == ".":
        out_path = Path(BUILD_DIR) / "docs" / stem / "index.html"
    else:
        out_path = Path(BUILD_DIR) / "docs" / parent / stem / "index.html"

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w") as f:
        f.write(page_html)

    return out_path, True


# ============================================================================
# Landing Page Builder
# ============================================================================


def build_landing_page():
    """Build the landing page."""
    template_path = _get_script_dir() / TEMPLATES_DIR / "landing.html"
    with open(template_path) as f:
        template = f.read()

    year = datetime.now().year
    html = template.replace("{base_url}", BASE_URL).replace("{year}", str(year))

    out_path = Path(BUILD_DIR) / "index.html"
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w") as f:
        f.write(html)

    print(f"  Built landing page -> {out_path}")


# ============================================================================
# Static Asset Copier
# ============================================================================


def copy_static_assets(build_dir: Path):
    """Copy static assets to build directory."""
    # Copy images (dirs_exist_ok to preserve building images already copied)
    src_img = _get_script_dir() / STATIC_DIR / "img"
    dest_img = build_dir / "img"
    if src_img.exists():
        shutil.copytree(src_img, dest_img, dirs_exist_ok=True)
        print("  Copied static/img/")

    # Copy Doxygen output if it exists (generated separately)
    src_doxygen = _get_script_dir() / STATIC_DIR / "doxygen"
    if src_doxygen.exists():
        dest_doxygen = build_dir / "doxygen"
        shutil.copytree(src_doxygen, dest_doxygen, dirs_exist_ok=True)
        print("  Copied static/doxygen/")

    # Copy .nojekyll
    nojekyll = _get_script_dir() / STATIC_DIR / ".nojekyll"
    if nojekyll.exists():
        shutil.copy2(nojekyll, build_dir / ".nojekyll")

    # Copy CSS
    css_dir = build_dir / "css"
    css_dir.mkdir(parents=True, exist_ok=True)
    for f in (_get_script_dir() / ASSETS_DIR / "css").iterdir():
        if f.is_file():
            shutil.copy2(f, css_dir / f.name)
    print("  Copied CSS assets")

    # Copy JS
    js_dir = build_dir / "js"
    js_dir.mkdir(parents=True, exist_ok=True)
    for f in (_get_script_dir() / ASSETS_DIR / "js").iterdir():
        if f.is_file():
            shutil.copy2(f, js_dir / f.name)
    print("  Copied JS assets")


# ============================================================================
# Main Build Orchestrator
# ============================================================================


def main():
    parser = argparse.ArgumentParser(description="Build the Ocean documentation site")
    parser.add_argument(
        "--skip-fetch",
        action="store_true",
        help="Skip fetching remote build guides from GitHub",
    )
    parser.add_argument(
        "--with-doxygen",
        action="store_true",
        help="Run Doxygen after building docs",
    )
    parser.add_argument(
        "--serve",
        action="store_true",
        help="Start a local HTTP server after build",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=3000,
        help="Port for local server (default: 3000)",
    )
    args = parser.parse_args()

    os.chdir(_get_script_dir())
    build_dir = Path(BUILD_DIR)

    print("Building Ocean documentation site...")
    start_time = datetime.now()

    # 1. Clean build directory
    if build_dir.exists():
        shutil.rmtree(build_dir)
    build_dir.mkdir(parents=True)
    print("1. Cleaned build directory")

    # 2. Fetch remote build guides
    if not args.skip_fetch:
        print("2. Fetching remote build guides...")
        results = fetch_remote_docs(_get_script_dir() / DOCS_DIR)
        failures = [r for r in results if not r[1]]
        if failures:
            print(f"  WARNING: {len(failures)} fetch(es) failed")
    else:
        print("2. Skipped fetching remote build guides")

    # 3. Copy building images
    print("3. Copying building images...")
    copy_building_images(build_dir)

    # 4. Build sidebar tree
    print("4. Building sidebar tree...")
    docs_root = _get_script_dir() / DOCS_DIR
    sidebar_tree, flat_pages = build_sidebar(docs_root)
    print(f"  Found {len(flat_pages)} pages")

    # 5. Load template
    template_path = _get_script_dir() / TEMPLATES_DIR / "base.html"
    with open(template_path) as f:
        template = f.read()

    # 6. Process all doc pages
    print("5. Processing documentation pages...")
    md_converter = create_markdown_converter()
    page_count = 0
    errors = []

    for md_file in sorted(docs_root.rglob("*.md")):
        # Skip meta-only docs
        if "fb" in md_file.relative_to(docs_root).parts:
            continue

        rel = md_file.relative_to(docs_root)
        try:
            out_path, _ = build_page(
                md_file, docs_root, template, sidebar_tree, flat_pages, md_converter
            )
            print(f"  {rel} -> {out_path}")
            page_count += 1
        except Exception as e:
            print(f"  ERROR: {rel}: {e}")
            errors.append((rel, str(e)))

    # 7. Generate landing page
    print("6. Generating landing page...")
    build_landing_page()

    # 8. Doxygen (optional, must run before copy_static_assets so that
    #    the generated files in static/doxygen/ get copied to build/)
    if args.with_doxygen:
        print("7. Running Doxygen...")
        doxygen_cfg = (
            _get_script_dir().parent
            / "build"
            / "doc"
            / "doxygen"
            / "ocean"
            / "doxygen-website.cfg"
        )
        if doxygen_cfg.exists():
            try:
                subprocess.run(
                    ["doxygen", str(doxygen_cfg)],
                    cwd=_get_script_dir().parent,
                    check=True,
                )
                print("  Doxygen completed")
            except FileNotFoundError:
                print("  WARNING: doxygen not found in PATH")
            except subprocess.CalledProcessError as e:
                print(f"  WARNING: doxygen failed: {e}")
        else:
            print(f"  WARNING: {doxygen_cfg} not found")

    # 9. Copy static assets (includes doxygen output from static/doxygen/)
    print("8. Copying static assets...")
    copy_static_assets(build_dir)

    # Summary
    elapsed = (datetime.now() - start_time).total_seconds()
    print(
        f"\nDone! Built {page_count} pages in {elapsed:.1f}s. {len(errors)} error(s)."
    )

    if errors:
        print("\nErrors:")
        for path, err in errors:
            print(f"  {path}: {err}")
        if not args.serve:
            sys.exit(1)

    # Serve
    if args.serve:
        if errors:
            print(
                f"\n*** WARNING: Serving site with {len(errors)} build error(s). "
                f"Some pages may be missing or broken. ***"
            )
        print(f"\nServing at http://localhost:{args.port}{BASE_URL}")
        print("Press Ctrl+C to stop.\n")
        os.chdir(build_dir)

        class Handler(http.server.SimpleHTTPRequestHandler):
            def translate_path(self, path):
                # Strip base URL prefix for local serving
                if path.startswith(BASE_URL):
                    path = "/" + path[len(BASE_URL) :]
                return super().translate_path(path)

            def log_message(self, format, *args):
                pass  # Suppress log noise

        server = http.server.HTTPServer(("", args.port), Handler)
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")


if __name__ == "__main__":
    main()
