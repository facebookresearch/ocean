# Building for macOS

This document describes the process to build Ocean for macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* XCode is required (recommended version: 15 or higher)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_linuxunix.sh`](build/cmake/build_thirdparty_linuxunix.sh). By default, this will build all third-party libraries in both debug and release configurations. Each of these will be built with static as well as dynamic linking. To build a specific configuration, use the parameters `--config BUILD_TYPE` and `--link LINKING_TYPE`. For example to build only the static debug and release configurations, run:

```
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Once the build is complete, there will be one subdirectory per build config within the installation and build directories. For example, for macOS it will similar to `${HOME}/install_ocean_thirdparty/macos_static_Debug` and `.../macos_static_Release`.

## 3 Building Ocean

The easiest way to build the third-party libraries is by using the provided build scripts. In a terminal, run:

```
cd ${OCEAN_DEVELOPMENT_PATH}
./build/cmake/build_ocean_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Make sure that the parameter specifying the location of the third-party libraries is the same as installation path from the previous section. Once the build is complete, the compiled binaries can be found in `${HOME}/install_ocean/static_Debug` and `.../static_Release`.
