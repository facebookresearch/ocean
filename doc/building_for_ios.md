# Building for iOS

This document describes the process of building Ocean for iOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Using Xcode to build the iOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Python 3.8 or higher
* Create an [Apple Developer account](https://developer.apple.com/), if you haven't already
* Retrieve the team ID of the above developer account. This will be an alphanumeric identifier of the form `XXXXXXXXXX`. Here are instructions to
  * [find it on the Apple page](https://developer.apple.com/help/account/manage-your-team/locate-your-team-id) and to
  * [find it in your local keychain](https://stackoverflow.com/a/47732584)
* Install a recent version of [Xcode](https://developer.apple.com/xcode/) (recommended version is Xcode 15).
* Install all required resources to build iOS following these [instructions](https://developer.apple.com/documentation/safari-developer-tools/installing-xcode-and-simulators).
* This document assumes the use of a physical iPhone with ARM64-based CPUs, not a simulator. Building for old iPhones should be possible but hasn't been tested.

## 2 Building the third-party libraries

As with the desktop use case, this process consists of two steps:

1. Building the required third-party libraries
2. Building the Ocean libraries

The third-party libraries are built using the Python-based build system. It handles fetching, patching, and building all dependencies with DAG-based parallel builds.

```bash
cd /path/to/ocean

# Build all required third-party libraries for iOS (debug + release, static)
python build/python/build_ocean_3rdparty.py --target ios

# Build release only
python build/python/build_ocean_3rdparty.py --target ios --config release

# Build for both iOS and macOS at the same time
python build/python/build_ocean_3rdparty.py --target ios --target macos

# Show build plan without building
python build/python/build_ocean_3rdparty.py --target ios --dry-run
```

Once the build is complete, the installed libraries can be found in `ocean_3rdparty/install/`. Headers are stored in `<lib>/h/ios/` and libraries in `<lib>/lib/ios_arm64_static_release/` (and `..._debug/`).

Run `python build/python/build_ocean_3rdparty.py --help` to see all available options.

> **Note:** The build system displays a real-time TUI with progress for all parallel build jobs. Use `--log-level verbose` to see detailed build output instead.

> **Note:** Shared library builds are not supported for iOS and will be automatically skipped.

## 3 Using Ocean in external Xcode projects

This section provides an example of how to build the Ocean libraries for the case that you plan to integrate them into an existing iOS project.

Make sure that the third-party libraries have been built and installed as described above.

```bash
cd /path/to/ocean

# Build Ocean for iOS
python build/python/build_ocean.py --target ios_arm64
# Build for a specific configuration
python build/python/build_ocean.py --target ios_arm64 --config release

# Specify custom directories
python build/python/build_ocean.py --target ios_arm64 \
    --build-dir "${HOME}/build_ocean" \
    --install-dir "${HOME}/install_ocean" \
    --third-party-dir /path/to/ocean_3rdparty/install
```

Once the build is complete, the compiled binaries can be found in `ocean_install/ios_arm64_static_release` (or with `_debug` suffix for debug builds).

Run `python build/python/build_ocean.py --help` to see all available options.

### Using CMake Directly

Alternatively, you can invoke CMake directly:

```bash
cd /path/to/ocean

# Configure and build
cmake -S . -B build_ios \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=build/python/toolchains/ios.toolchain.cmake \
    -DPLATFORM=OS64 \
    -DDEPLOYMENT_TARGET=15.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install

cmake --build build_ios --target install
```

From here, the Ocean binaries and include files can be used in any other project. Check out this guide on how to [include the CMake project of Ocean in an Xcode project](https://blog.tomtasche.at/2019/05/how-to-include-cmake-project-in-xcode.html).

## 4 Building the Ocean iOS demo/test apps

First, build the required third-party libraries as described above (targets should be available as static libraries as debug and/or release builds).

Also have your Apple team ID ready; it should have the following format: `XXXXXXXXXX`. Without it the apps cannot be signed and the build will fail.

### Option A: Using the Build Script (Recommended)

Build Ocean and then open the generated Xcode project:

```bash
cd /path/to/ocean

# Configure only (to generate Xcode project)
python build/python/build_ocean.py --target ios_arm64 --configure-only

# Open in Xcode (adjust the path to match your build config)
open ocean_build/ios_arm64_static_debug/ocean.xcodeproj
```

### Option B: Manual CMake Configuration

To configure the CMake project of Ocean as a debug build manually:

```bash
# Debug
cd /path/to/ocean
cmake -S . \
    -B "${HOME}/build_ocean_ios_debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=build/python/toolchains/ios.toolchain.cmake \
    -DPLATFORM="OS64" \
    -DDEPLOYMENT_TARGET="15.0" \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DBUILD_SHARED_LIBS="OFF"
```

and for release builds:

```bash
# Release
cd /path/to/ocean
cmake -S . \
    -B "${HOME}/build_ocean_ios_release" \
    -DCMAKE_BUILD_TYPE="Release" \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=build/python/toolchains/ios.toolchain.cmake \
    -DPLATFORM="OS64" \
    -DDEPLOYMENT_TARGET="15.0" \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DBUILD_SHARED_LIBS="OFF"
```

In both cases make sure to replace `XXXXXXXXXX` with your Apple Team ID.

Once the configuration is complete, open the generated Xcode project:

```bash
# Debug
open ${HOME}/build_ocean_ios_debug/ocean.xcodeproj
```

or

```bash
# Release
open ${HOME}/build_ocean_ios_release/ocean.xcodeproj
```

If Xcode asks whether schemes should be created automatically or manually, select automatically.

Then search for the demo/test apps at the top of the Xcode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._ios`. Select an app and build and install as normal. For inspiration, check out the list of available demos on the [project website](https://facebookresearch.github.io/ocean/docs/introduction/).

Ocean contains many demo and test apps. So, if you're using an unpaid Apple developer account, you may run into the following limitation:

> Communication with Apple failed. Your maximum App ID limit has been reached. You may create up to 10 App IDs every 7 days.
