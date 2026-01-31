# Building for iOS

This document describes the process of building Ocean for iOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Using Xcode to build the iOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* CMake 3.25 or higher is required (for CMake preset support)
* Create an [Apple Developer account](https://developer.apple.com/), if you haven't already
* Retrieve the team ID of the above developer account. This will be an alphanumeric identifier of the form `XXXXXXXXXX`. Here are instructions to
  * [find it on the Apple page](https://developer.apple.com/help/account/manage-your-team/locate-your-team-id) and to
  * [find it in your local keychain](https://stackoverflow.com/a/47732584)
* Install a recent version of [Xcode](https://developer.apple.com/xcode/) (recommended version is Xcode 15).
* Install all required resources to build iOS following these [instructions](https://developer.apple.com/documentation/safari-developer-tools/installing-xcode-and-simulators).
* This document assumes the use a physical iPhone with ARM64-based CPUs, not a simulator. Building for old iPhones should be possible but hasn't been tested. For details check the toolchain file, [build/cmake/ios-cmake/ios.toolchain.cmake](https://github.com/facebookresearch/ocean/blob/7eb6a04e60278185c3ebe469829f00fc7092b223/build/cmake/ios-cmake/ios.toolchain.cmake#L59-L60).

## 2 Building the third-party libraries

As with the desktop use case, this process consists of two steps:

1. Building the required third-party libraries
2. Building the Ocean libraries

The easiest way to build the third-party libraries is by using the provided build script. By default, this will build all third-party libraries in both debug and release configurations with static linking.

```
cd /path/to/ocean
./build/cmake/build_thirdparty_ios.sh
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/3rdparty/ios/arm64_static_debug` and `.../ios/arm64_static_release`.

The build script can be customized using command-line parameters. Use `--config` to specify build configurations, `--link` for linking type, `-b` for build directory, and `-i` for installation directory. For example:

```
cd /path/to/ocean
./build/cmake/build_thirdparty_ios.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_thirdparty_ios.sh --help` to see all available options.

> **Note:** By default, the build scripts only display error messages. To see more detailed CMake output, use `--log-level STATUS` (for general progress information) or other levels like `VERBOSE` or `DEBUG`.

## 3 Using Ocean in external Xcode projects

This section provides an example of how to build the Ocean libraries for the case that you plan to integrate them into an existing iOS project.

Make sure that the third-party libraries have been built and installed as described above. Ocean uses CMake presets for build configuration. The unified build script [`build/cmake/build_ocean.sh`](build/cmake/build_ocean.sh) supports cross-compilation for iOS from macOS. By default, the script will look for third-party libraries in `bin/cmake/3rdparty` (the default output from the previous step).

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -p ios
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/ios/arm64_static_release` (or with `_debug` suffix for debug builds).

The build script can be customized using command-line parameters. For example:

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -p ios -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_ocean.sh --help` to see all available options.

### Using CMake Presets Directly

Alternatively, you can use CMake presets directly without the build script:

```bash
# List all available presets
cmake --list-presets

# Configure and build using a preset
cmake --preset ios-arm64-static-release -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/ios/arm64_static_release"
cmake --build --preset ios-arm64-static-release --target install
```

From here, the Ocean binaries and include files can be used in any other project. Check out this guide on how to [include the CMake project of Ocean in an Xcode project](https://blog.tomtasche.at/2019/05/how-to-include-cmake-project-in-xcode.html).

## 4 Building the Ocean iOS demo/test apps

First, build the required third-party libraries as described above (targets should be available as static libraries as debug and/or release builds).

Also have your Apple team ID ready; it should have the following format: `XXXXXXXXXX`. Without it the apps cannot be signed and the build will fail.

### Option A: Using CMake Presets (Recommended)

Configure the project using a CMake preset:

```bash
# Debug
cd /path/to/ocean
cmake --preset ios-arm64-static-debug \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/ios/arm64_static_debug" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX

# Open in Xcode
open bin/cmake/tmp/ios-arm64-static-debug/ocean.xcodeproj
```

For release builds:

```bash
# Release
cmake --preset ios-arm64-static-release \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/ios/arm64_static_release" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX

open bin/cmake/tmp/ios-arm64-static-release/ocean.xcodeproj
```

Make sure to replace `XXXXXXXXXX` with your Apple Team ID.

### Option B: Manual CMake Configuration

To configure the CMake project of Ocean as a debug build manually:

```
# Debug
cd /path/to/ocean
cmake -S"/path/to/ocean" \
    -B"${HOME}/build_ocean_ios_debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE="/path/to/ocean/build/cmake/ios-cmake/ios.toolchain.cmake" \
    -DPLATFORM="OS64" \
    -DDEPLOYMENT_TARGET="15.0" \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/ios/arm64_static_debug" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DBUILD_SHARED_LIBS="OFF"
```

and for release builds:

```
# Release
cd /path/to/ocean
cmake -S"/path/to/ocean" \
    -B"${HOME}/build_ocean_ios_release" \
    -DCMAKE_BUILD_TYPE="Release" \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE="/path/to/ocean/build/cmake/ios-cmake/ios.toolchain.cmake" \
    -DPLATFORM="OS64" \
    -DDEPLOYMENT_TARGET="15.0" \
    -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/ios/arm64_static_release" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DBUILD_SHARED_LIBS="OFF"
```

In both cases make sure to replace `XXXXXXXXXX` with your Apple Team ID.

Once the configuration is complete, open the generated Xcode project:

```
# Debug
open ${HOME}/build_ocean_ios_debug/ocean.xcodeproj
```

or

```
# Release
open ${HOME}/build_ocean_ios_release/ocean.xcodeproj
```

If Xcode asks whether schemes should be created automatically or manually, select automatically.

Then search for the demo/test apps at the top of the Xcode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._ios`. Select an app and build and install as normal. For inspiration checkout the list of available demos on the [project website](https://facebookresearch.github.io/ocean/docs/introduction/).

Ocean contains many demo and test apps. So, if you're using an unpaid Apple developer account, you may run into the following limitation:

> Communication with Apple failed. Your maximum App ID limit has been reached. You may create up to 10 App IDs every 7 days.
