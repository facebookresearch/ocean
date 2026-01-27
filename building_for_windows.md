# Building for Windows

This document describes the process to build Ocean for Windows. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Visual Studio 2022 is required

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_windows.bat`](build/cmake/build_thirdparty_windows.bat). By default, this will build all third-party libraries in both debug and release configurations with static linking.

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat
```

Once the build is complete, there will be one subdirectory per build config within the installation directory `ocean_install_thirdparty` in the current directory.

The build script can be customized using command-line parameters. Use `-config` to specify build configurations, `-link` for linking type, `-build` for build directory, and `-install` for installation directory. For example:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat -config debug,release -link static -build C:\build_oceanTP -install C:\install_oceanTP
```

It is advisable to place build and install directories as close to the root of a filesystem as possible, due to Windows limitations on path lengths. Build directories in particular can be very deep.

Run `.\build\cmake\build_thirdparty_windows.bat -help` to see all available options.

## 3 Building Ocean

The easiest way to build all Ocean libraries and apps is by using the provided build script. By default, it will look for third-party libraries in `ocean_install_thirdparty` (the default output from the previous step).

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_ocean_windows.bat
```

The build script can be customized using command-line parameters. For example:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_ocean_windows.bat -config debug,release -link static -third-party C:\install_oceanTP
```

Run `.\build\cmake\build_ocean_windows.bat -help` to see all available options.
