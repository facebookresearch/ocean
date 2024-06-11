# Building for macOS

This document describes the process to build Ocean for macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* XCode is required (recommended version: 15 or higher)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_linuxunix.sh`](build/cmake/build_thirdparty_linuxunix.sh). By default, this will build all third-party libraries in both debug and release configurations. Each of these will be built with static as well as dynamic linking. To build a specific configuration, use the parameters `--config BUILD_TYPE` and `--link LINKING_TYPE`. For example to build only the shared debug configurations, run:

```
./build/cmake/build_thirdparty_linuxunix.sh --config debug --link shared
```

The default installation location will be shown in the logs of the build script and similar to `/tmp/ocean/install/macos`. A custom location for the installation can specified as follows:

```
./build/cmake/build_thirdparty_linuxunix.sh --install /path/to/install/root/dir
```

Methods for specifying the installation directory, in descending order of preference are:
1. Using `--install /path/to/install/root/dir`
2. Using the environment variable `OCEAN_INSTALL_PATH`
3. Change the hard-coded default in the build script, `/tmp/ocean/install`

Similarly, the root directory where the code should be built can be specified as follows:

```
./build/cmake/build_thirdparty_linuxunix.sh --build /path/to/build/root
```

Otherwise the code will be built in `/tmp/ocean/build/macos`.

Within the installation and build directories, there will be one subdirectory per build config.

## 3 Building Ocean
