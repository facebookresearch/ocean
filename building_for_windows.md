# Building for Windows

This document describes the process to build Ocean for Windows. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* CMake 3.25 or higher is required (for CMake preset support)
* Visual Studio 2019 or later is required (2022 recommended). By default, CMake will auto-detect the newest installed version.

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_windows.bat`](build/cmake/build_thirdparty_windows.bat). By default, this will build all third-party libraries in both debug and release configurations with static linking.

```
cd /D \path\to\ocean
.\build\cmake\build_thirdparty_windows.bat
```

Once the build is complete, the compiled binaries can be found in `ocean_install_thirdparty\windows\x64_vc145_static_debug` and `...\windows\x64_vc145_static_release` (where `vc145` corresponds to the Visual Studio toolset version, e.g., `vc143` for VS 2022, `vc145` for VS 2026; or with `arm64_` prefix on ARM64 systems).

The build script can be customized using command-line parameters. Use `-config` to specify build configurations, `-link` for linking type, `-build` for build directory, and `-install` for installation directory. For example:

```
cd /D \path\to\ocean
.\build\cmake\build_thirdparty_windows.bat -config debug,release -link static -build C:\build_oceanTP -install C:\install_oceanTP
```

It is advisable to place build and install directories as close to the root of a filesystem as possible, due to Windows limitations on path lengths. Build directories in particular can be very deep.

Run `.\build\cmake\build_thirdparty_windows.bat -help` to see all available options.

To use a specific Visual Studio version instead of the auto-detected one:

```
.\build\cmake\build_thirdparty_windows.bat -generator "Visual Studio 16 2019"
```

> **Note:** By default, the build scripts only display error messages. To see more detailed CMake output, use `-log-level STATUS` (for general progress information) or other levels like `VERBOSE` or `DEBUG`.

## 3 Building Ocean

Ocean uses CMake presets for build configuration. There are two options for building Ocean on Windows:

### Option A: Using PowerShell (Recommended)

The easiest way to build all Ocean libraries and apps is by using the PowerShell script [`build/cmake/build_ocean.ps1`](build/cmake/build_ocean.ps1). By default, it will look for third-party libraries in `ocean_install_thirdparty` (the default output from the previous step).

```powershell
cd \path\to\ocean
.\build\cmake\build_ocean.ps1
```

Once the build is complete, the compiled binaries can be found in `ocean_install\windows\x64_vc145_static_debug` and `...\windows\x64_vc145_static_release` (where `vc145` corresponds to the Visual Studio toolset version; or with `arm64_` prefix on ARM64 systems).

The build script can be customized using command-line parameters. For example:

```powershell
cd \path\to\ocean
.\build\cmake\build_ocean.ps1 -Config debug,release -Link static -Build C:\build_ocean -Install C:\install_ocean -ThirdParty C:\install_oceanTP
```

Run `Get-Help .\build\cmake\build_ocean.ps1 -Detailed` to see all available options.

To use a specific Visual Studio version instead of the auto-detected one:

```powershell
.\build\cmake\build_ocean.ps1 -Generator "Visual Studio 16 2019"
```

### Option B: Using Git Bash

You can also use the unified bash script via Git Bash:

```bash
cd /path/to/ocean
./build/cmake/build_ocean.sh
```

The bash script supports the same options as on other platforms. Run `./build/cmake/build_ocean.sh --help` for details.

To use a specific Visual Studio version:

```bash
./build/cmake/build_ocean.sh -g "Visual Studio 16 2019"
```

### Using CMake Presets Directly

Alternatively, you can use CMake presets directly without the build scripts:

```powershell
# List all available presets
cmake --list-presets

# Configure using a preset
cmake --preset windows-x64-static-release -DCMAKE_PREFIX_PATH="C:\install_oceanTP\windows\x64_static_release"

# Build and install
cmake --build --preset windows-x64-static-release --target install
```

To use a specific Visual Studio version with presets, add the `-G` flag:

```powershell
cmake --preset windows-x64-static-release -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="C:\install_oceanTP\windows\x64_static_release"
```

### Building with Visual Studio

To open the project in Visual Studio after configuration:

```powershell
# Configure the project
cmake --preset windows-x64-static-release -DCMAKE_PREFIX_PATH="C:\install_oceanTP\windows\x64_static_release"

# Open in Visual Studio
start ocean_build\windows-x64-static-release\ocean.sln
```

Then build and run the desired targets from within Visual Studio.
