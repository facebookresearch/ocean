# Building for macOS

This document describes the process to build Ocean for macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean
4. Using XCode to build the macOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* CMake 3.25 or higher is required (for CMake preset support)
* XCode is required (recommended version: 15 or higher)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_linuxunix.sh`](build/cmake/build_thirdparty_linuxunix.sh). This script handles both Linux and macOS builds and will automatically detect the platform. By default, it will build all third-party libraries in both debug and release configurations with static linking.

```
cd /path/to/ocean
./build/cmake/build_thirdparty_linuxunix.sh
```

Once the build is complete, there will be one subdirectory per build config within the installation and build directories. For example, on an Apple Silicon Mac it will be similar to `ocean_install_thirdparty/macos/arm64_static_debug` and `.../macos/arm64_static_release`. On Intel Macs, the folder will use `x64` instead of `arm64`.

The build script can be customized using command-line parameters. Use `--config` to specify build configurations, `--link` for linking type, `-b` for build directory, and `-i` for installation directory. For example:

```
cd /path/to/ocean
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_thirdparty_linuxunix.sh --help` to see all available options.

> **Note:** By default, the build scripts only display error messages. To see more detailed CMake output, use `--log-level STATUS` (for general progress information) or other levels like `VERBOSE` or `DEBUG`.

## 3 Building Ocean

Ocean uses CMake presets for build configuration. The easiest way to build all Ocean libraries and apps is by using the provided build script, [`build/cmake/build_ocean.sh`](build/cmake/build_ocean.sh). By default, it will look for third-party libraries in `ocean_install_thirdparty` (the default output from the previous step).

```
cd /path/to/ocean
./build/cmake/build_ocean.sh
```

Once the build is complete, the compiled binaries can be found in `ocean_install/macos/arm64_static_debug` and `.../macos/arm64_static_release` (or `x64_static_*` on Intel Macs).

The build script can be customized using command-line parameters. For example:

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_ocean.sh --help` to see all available options.

### Using CMake Presets Directly

Alternatively, you can use CMake presets directly without the build script:

```bash
# List all available presets
cmake --list-presets

# Configure and build using a preset
cmake --preset macos-arm64-static-release -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/macos/arm64_static_release"
cmake --build --preset macos-arm64-static-release --target install
```

## 4 Building the Ocean macOS demo/test apps

First, build the required third-party libraries as described above (targets should be available as static libraries as debug and/or release builds).

### Option A: Using CMake Presets (Recommended)

Configure the project using a CMake preset and open in Xcode:

```bash
# Debug
cd /path/to/ocean
cmake --preset macos-arm64-static-debug \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/macos/arm64_static_debug"

# Open in Xcode
open ocean_build/macos-arm64-static-debug/ocean.xcodeproj
```

For release builds:

```bash
# Release
cmake --preset macos-arm64-static-release \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/macos/arm64_static_release"

open ocean_build/macos-arm64-static-release/ocean.xcodeproj
```

### Option B: Manual CMake Configuration

To configure the CMake project of Ocean as a debug build manually:

```
# Debug
cd /path/to/ocean
cmake -S"/path/to/ocean" \
    -B"${HOME}/build_ocean_macos_debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/macos/arm64_static_debug" \
    -DBUILD_SHARED_LIBS="OFF"
```

and for release builds:

```
# Release
cd /path/to/ocean
cmake -S"/path/to/ocean" \
    -B"${HOME}/build_ocean_macos_release" \
    -DCMAKE_BUILD_TYPE="Release" \
    -G Xcode \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/macos/arm64_static_release" \
    -DBUILD_SHARED_LIBS="OFF"
```

Once the configuration is complete, open the generated Xcode project:

```
# Debug
open ${HOME}/build_ocean_macos_debug/ocean.xcodeproj
```

or

```
# Release
open ${HOME}/build_ocean_macos_release/ocean.xcodeproj
```

If Xcode asks whether schemes should be created automatically or manually, select automatically.

Then search for the demo/test apps at the top of the Xcode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._osx`. Select an app and build and install as normal. For inspiration checkout the list of available demos on the [project website](https://facebookresearch.github.io/ocean/docs/introduction/).
