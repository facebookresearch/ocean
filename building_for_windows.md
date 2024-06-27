# Building for Windows

This document describes the process to build Ocean for Windows. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Visual Studio 2022 is required

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_windows.bat`](build/cmake/build_thirdparty_windows.bat). By default, this will build all third-party libraries in both debug and release configurations. Each of these will be built with static as well as dynamic linking. To build a specific configuration, use the parameters `-config BUILD_TYPE` and `-link LINKING_TYPE`. For example to build only the static debug configurations, run:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat -config debug -link static
```

The installation location will be shown in the logs of the build script and defaults to a directory `ocean_install_thirdparty` in the current directory. A custom location for the installation can specified as follows:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat -install X:\path\to\install\root\dir
```

Methods for specifying the installation directory, in descending order of preference are:
1. Using `-install X:\path\to\install\root\dir`
2. Change the hard-coded default (`set DEFAULT_INSTALL_PATH=C:\tmp\ocean\install\win`) in the build script files

Similarly, the root directory where the code should be built can be specified as follows:

```
.\build\cmake\build_thirdparty_windows.bat -build X:\path\to\build\root
```

Otherwise the code will be built in `ocean_build_thirdparty` in the current directory. Once the build is finished, this build directory can be discarded.

Within the installation and build directories, there will be one subdirectory per build config.

## 3 Building Ocean

The provided build script [`build/cmake/build_ocean_windows.bat`](build/cmake/build_ocean_windows.bat) will build all Ocean libraries and apps. It supports the same command-line configuration options as the third-party library build script. If the third-party dependencies were installed somewhere other than `ocean_install_thirdparty` in the current directory the Ocean build script needs to be informed, using the `-third-party` option, where to find them. For example to build only the static debug configurations, run:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_ocean_windows.bat -config debug -link static -third-party X:\path\to\third-party\install\dir
```
