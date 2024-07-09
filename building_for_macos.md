# Building for macOS

This document describes the process to build Ocean for macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean
4. Using XCode to build the macOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* XCode is required (recommended version: 15 or higher)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_linuxunix.sh`](build/cmake/build_thirdparty_linuxunix.sh). By default, this will build all third-party libraries in both debug and release configurations. Each of these will be built with static as well as dynamic linking. To build a specific configuration, use the parameters `--config BUILD_TYPE` and `--link LINKING_TYPE`. For example to build only the static debug and release configurations, run:

```
cd ${OCEAN_DEVELOPMENT_PATH}
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Once the build is complete, there will be one subdirectory per build config within the installation and build directories. For example, for macOS it will similar to `${HOME}/install_ocean_thirdparty/macos_static_Debug` and `.../macos_static_Release`.

## 3 Building Ocean

The easiest way to build all Ocean libraries and apps is by using the provided build scripts. In a terminal, run:

```
cd ${OCEAN_DEVELOPMENT_PATH}
./build/cmake/build_ocean_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Make sure that the parameter specifying the location of the third-party libraries is the same as installation path from the previous section. Once the build is complete, the compiled binaries can be found in `${HOME}/install_ocean/static_Debug` and `.../static_Release`.

## 4 Building the Ocean macOS demo/test apps

First, build the required third-party libraries as described above with (targets should be available as static libraries as debug and/or release builds).

To configure the CMake project of Ocean as a debug build, use:

```
# Debug
cd ${OCEAN_DEVELOPMENT_PATH}
cmake -S"${OCEAN_DEVELOPMENT_PATH}" \
    -B"${HOME}/build_ocean_macos_debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DCMAKE_INSTALL_PREFIX="${HOME}/install_ocean_thirdparty/macos_static_Debug" \
    -DBUILD_SHARED_LIBS="OFF"
```

and for release builds, use:

```
# Release
cd ${OCEAN_DEVELOPMENT_PATH}
cmake -S"${OCEAN_DEVELOPMENT_PATH}" \
    -B"${HOME}/build_ocean_macos_release" \
    -DCMAKE_BUILD_TYPE="Release" \
    -G Xcode \
    -DCMAKE_INSTALL_PREFIX="${HOME}/install_ocean_thirdparty/macos_static_Release" \
    -DBUILD_SHARED_LIBS="OFF"
```
Once the configuration is complete, open the generated the XCode project:

```
# Debug
open ${HOME}/build_ocean_macos_debug/ocean.xcodeproj
```

or

```
# Release
open ${HOME}/build_ocean_macos_release/ocean.xcodeproj
```

If XCode asks whether schemes should be created automatically or manually, select automatically.

Then search for the demo/test apps at the top of the XCode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._osx`. Select an app and build and install as normal. For inspiration checkout the list of available demos on the [project website](https://facebookresearch.github.io/ocean/docs/introduction/).
