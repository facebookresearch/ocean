# Building for macOS

This document describes the process to build Ocean for macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean
4. Using Xcode to build the macOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](../README.md)
* Python 3.10 or higher, plus the build scripts' dependencies: `pip install -r build/python/requirements.txt`
* Xcode is required (recommended version: 15 or higher)

## 2 Building the third-party libraries

The third-party libraries are built using the Python-based build system. It handles fetching, patching, and building all dependencies with DAG-based parallel builds.

```bash
cd /path/to/ocean

# Build for every target this host supports (on macOS: macOS, iOS and Android if their SDKs are installed)
python build/python/build_ocean_3rdparty.py

# Build for macOS only (both arm64 and x86_64)
python build/python/build_ocean_3rdparty.py --target macos

# Build release only
python build/python/build_ocean_3rdparty.py --target macos --config release

# Build shared libraries
python build/python/build_ocean_3rdparty.py --target macos --link shared

# Include optional libraries (e.g., OpenCV)
python build/python/build_ocean_3rdparty.py --target macos --with opencv

# Show build plan without building
python build/python/build_ocean_3rdparty.py --target macos --dry-run
```

Once the build is complete, the installed libraries can be found in `ocean_3rdparty/install/`. Each library is a complete, relocatable CMake install prefix at `<target>/<library>/`, for example `macos_arm64_static/zlib/include/zlib.h` and `macos_arm64_static/zlib/lib/libz.a`. Release targets have no suffix; debug targets add `_debug` (e.g., `macos_arm64_static_debug`).

Passing `--for-external-integration` produces a different, flattened layout intended for non-CMake build systems, with headers shared across architectures: `<library>/h/<platform>/` and `<library>/lib/<target>/`.

Run `python build/python/build_ocean_3rdparty.py --help` to see all available options.

> **Note:** The build system displays a real-time TUI with progress for all parallel build jobs. Use `--log-level verbose` to see detailed build output instead.

## 3 Building Ocean

Ocean is built using a Python build script that invokes CMake with the correct configuration.

```bash
cd /path/to/ocean

# Build Ocean using the new Python 3P layout
python build/python/build_ocean.py
# Build for a specific configuration
python build/python/build_ocean.py --config release

# Specify a custom third-party directory
python build/python/build_ocean.py --third-party-dir /path/to/ocean_3rdparty/install

# Show build plan without building
python build/python/build_ocean.py --dry-run
```

Once the build is complete, the compiled binaries can be found in `ocean_install/macos_arm64_static` (release) and `.../macos_arm64_static_debug` (or `macos_x86_64_static*` on Intel Macs).

Run `python build/python/build_ocean.py --help` to see all available options.

### Using CMake Directly

Alternatively, you can invoke CMake directly:

```bash
cd /path/to/ocean

# Configure
cmake -S . -B build_macos \
    -G Xcode \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DCMAKE_INSTALL_PREFIX=./ocean_install/macos_arm64_static

# Build. Xcode is a multi-config generator, so --config must repeat the
# configuration; without it Xcode builds Debug against the release libraries.
cmake --build build_macos --config Release --target install --parallel 8
```

## 4 Building the Ocean macOS demo/test apps

First, build the required third-party libraries as described above (targets should be available as static libraries as debug and/or release builds).

### Option A: Using the Build Script (Recommended)

Build Ocean and then open the generated Xcode project:

```bash
cd /path/to/ocean

# Configure only (to generate the Xcode project)
python build/python/build_ocean.py --configure-only --generator Xcode

# Open in Xcode
open ocean_build/macos_arm64_static_debug/ocean.xcodeproj
```

### Option B: Manual CMake Configuration

To configure the CMake project of Ocean as a debug build manually:

```bash
# Debug
cd /path/to/ocean
cmake -S . \
    -B "${HOME}/build_ocean_macos_debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DBUILD_SHARED_LIBS="OFF"
```

and for release builds:

```bash
# Release
cd /path/to/ocean
cmake -S . \
    -B "${HOME}/build_ocean_macos_release" \
    -DCMAKE_BUILD_TYPE="Release" \
    -G Xcode \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DBUILD_SHARED_LIBS="OFF"
```

Once the configuration is complete, open the generated Xcode project:

```bash
# Debug
open ${HOME}/build_ocean_macos_debug/ocean.xcodeproj
```

or

```bash
# Release
open ${HOME}/build_ocean_macos_release/ocean.xcodeproj
```

If Xcode asks whether schemes should be created automatically or manually, select automatically.

Then search for the demo/test apps at the top of the Xcode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._osx`. Select an app and build and install as normal. For inspiration, check out the list of available demos on the [project website](https://facebookresearch.github.io/ocean/docs/introduction/).
