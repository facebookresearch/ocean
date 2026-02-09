# Adding and Updating Libraries

This guide explains how to add new libraries or update existing ones in `dependencies.yaml`.

## Quick Reference

```bash
# Validate your changes
./validate_manifest.py

# Visualize the dependency graph
./visualize_deps.py

# Test build a single library
./build_ocean_3p.py --config debug --dry-run

# Build just one library (with dependencies)
./build_ocean_3p.py --config debug
```

## Adding a New Library

### 1. Find the Library Information

Gather:
- **Version**: Stable release version (e.g., `"1.3.1"`)
- **Source URL**: Git repository URL
- **Git Ref**: Tag, branch, or commit (prefer tags like `"v1.3.1"`)
- **Build System**: Usually `cmake` or `header_only`
- **Dependencies**: Other libraries it needs

### 2. Add to dependencies.yaml

Add a new entry in the `libraries` section:

```yaml
libraries:
  # ... existing libraries ...

  my-new-lib:
    version: "1.2.3"
    description: "Short description of what the library does"
    source:
      type: git
      url: https://github.com/owner/my-new-lib.git
      ref: "v1.2.3"
    build:
      system: cmake
      options:
        BUILD_TESTS: OFF
        BUILD_EXAMPLES: OFF
    dependencies:
      - zlib  # if it depends on zlib
    platforms: all
```

### 3. Determine Build Options

Most libraries need options to disable tests/examples/docs:

```yaml
# Common CMake options to try:
options:
  BUILD_TESTING: OFF
  BUILD_TESTS: OFF
  BUILD_EXAMPLES: OFF
  BUILD_DOCS: OFF
  BUILD_SHARED_LIBS: OFF  # Usually set automatically
```

Check the library's `CMakeLists.txt` for available options.

### 4. Validate

```bash
./validate_manifest.py
```

This checks:
- Schema compliance
- Dependency references exist
- No circular dependencies
- Valid URLs

### 5. Test Build

```bash
# Dry run first
./build_ocean_3p.py --dry-run

# Build debug only to test faster
./build_ocean_3p.py \
    --install-dir /tmp/test-install \
    --build-dir /tmp/test-build \
    --config debug
```

### 6. Fix Build Issues

Common issues and fixes:

| Issue | Solution |
|-------|----------|
| CMake version error | Add `CMAKE_POLICY_VERSION_MINIMUM: "3.5"` |
| Tests use x86 assembly | Add `BUILD_TESTING: OFF` |
| Uses bundled dependency | Add `USE_BUNDLED_X: OFF` or `X_BUILD_BUNDLED: OFF` |
| Can't find dependency | Check dependency is listed and builds first |

## Updating a Library Version

### 1. Update Version and Ref

```yaml
my-lib:
  version: "1.3.0"  # Update version
  source:
    ref: "v1.3.0"   # Update git ref
```

### 2. Check for Breaking Changes

- Review library's changelog
- Check if CMake options changed
- Verify dependencies haven't changed

### 3. Clean and Rebuild

```bash
# Remove cached source and build
rm -rf /tmp/test-build/my-lib /tmp/test-source/my-lib

# Rebuild
./build_ocean_3p.py --install-dir /tmp/test-install --config debug
```

## Library Types

### CMake Libraries (most common)

```yaml
build:
  system: cmake
  options:
    OPTION_NAME: value
```

### Header-Only Libraries

For libraries that are just headers (no compilation):

```yaml
build:
  system: header_only
```

The build system will just copy the `include/` directory.

## Optional Libraries

Mark libraries as optional if they're not always needed:

```yaml
opencv:
  optional: true
  optional_group: "vision"  # Group related optionals
  # ...
```

Build with: `./build_ocean_3p.py --with opencv` or `--with-group vision`

## Dependency Levels

Libraries are built in levels based on dependencies:

```
Level 0: zlib, eigen, googletest, ...  (no dependencies, build in parallel)
Level 1: libpng, curl, assimp, ...     (depend on Level 0)
Level 2: freetype, ...                  (depend on Level 0 and/or 1)
```

View levels with: `./visualize_deps.py --format levels`

## Schema Reference

See `schema/dependencies.schema.json` for the complete schema definition.

### Required Fields

```yaml
my-lib:
  version: "x.y.z"      # Required: version string
  source:               # Required: how to get the source
    type: git           # Required: git, archive, or local
    url: "..."          # Required for git
    ref: "..."          # Required for git
  build:                # Required: how to build
    system: cmake       # Required: cmake or header_only
```

### Optional Fields

```yaml
my-lib:
  description: "..."    # Human-readable description
  dependencies: [...]   # List of library names
  platforms: all        # all, or: [macos, linux, ios, android, windows]
  optional: false       # If true, not built by default
  optional_group: "..." # Group name for --with-group
  build:
    options: {...}      # CMake definitions
  source:
    patch: "..."        # Path to patch file (relative to manifest dir)
    copy_files: {...}   # Files to copy into source after checkout
```

## Patching Libraries

Some libraries need modifications to build correctly. The build system supports two methods:

### Applying Patches

Use the `patch` field to apply a git-style patch after checkout:

```yaml
libusb:
  source:
    type: git
    url: https://github.com/libusb/libusb-cmake.git
    ref: "cec1e49..."
    patch: patches/libusb.patch  # Relative to manifest directory
```

Store patch files in the `patches/` directory next to `dependencies.yaml`.

### Copying Files

Use `copy_files` to copy files into the source tree (useful for adding missing CMakeLists.txt):

```yaml
giflib:
  source:
    type: git
    url: https://git.code.sf.net/p/giflib/code
    ref: "5.2.2"
    copy_files:
      patches/giflib_CMakeLists.txt: CMakeLists.txt  # src -> dest
```

Files are copied before patches are applied, so patches can modify copied files.
