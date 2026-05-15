# Building for Windows

This document describes the process to build Ocean for Windows. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Python 3.8 or higher
* Visual Studio 2019 or later is required (2022 recommended). The build system will auto-detect the newest installed version.

### Enabling Long Path Support (Highly Recommended)

Windows traditionally has a 260-character path limit (MAX_PATH). Ocean's build process can generate deeply nested paths that may exceed this limit. It is **highly recommended** to enable Long Path support before building.

To enable Long Path support, run this command in an elevated (Administrator) PowerShell:

```powershell
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' -Name 'LongPathsEnabled' -Value 1
```

Alternatively, use the Group Policy Editor:
1. Open `gpedit.msc`
2. Navigate to: Computer Configuration > Administrative Templates > System > Filesystem
3. Enable: "Enable Win32 long paths"

A system restart may be required after enabling this setting.

> **Note:** The build system will automatically check for Long Path support and display a warning if it is not enabled.

## 2 Building the third-party libraries

The third-party libraries are built using the Python-based build system. The same script works from PowerShell, Command Prompt, or Git Bash.

```powershell
cd \path\to\ocean

# Build all required third-party libraries for the host platform (debug + release, static + shared)
python build/python/build_ocean_3rdparty.py

# Build for a specific architecture
python build/python/build_ocean_3rdparty.py --target win

# Build release only, static only
python build/python/build_ocean_3rdparty.py --config release --link static

# Use a specific Visual Studio version
python build/python/build_ocean_3rdparty.py --vs-version 2022

# Include optional libraries (e.g., OpenCV)
python build/python/build_ocean_3rdparty.py --with opencv

# Show build plan without building
python build/python/build_ocean_3rdparty.py --dry-run
```

It is advisable to place build and install directories as close to the root of a filesystem as possible, due to Windows limitations on path lengths:

```powershell
python build/python/build_ocean_3rdparty.py --output-dir C:\ocean_3rdparty
```

Once the build is complete, the installed libraries can be found in `ocean_3rdparty/install/` (or the custom output directory). Headers are stored in `<lib>/h/<platform>/` and libraries in `<lib>/lib/<target>/` (e.g., `zlib/lib/win_x86_64_static_release/`).

On Windows, the default is to build both static and shared libraries. On ARM64 systems, paths use `arm64` instead of `x86_64`.

Run `python build/python/build_ocean_3rdparty.py --help` to see all available options.

> **Note:** The build system displays a real-time TUI with progress for all parallel build jobs. Use `--log-level verbose` to see detailed build output instead.

## 3 Building Ocean

Ocean is built using a Python build script that invokes CMake with the correct configuration.

### Using the Build Script (Recommended)

```powershell
cd \path\to\ocean

# Build Ocean using the Python 3P layout
python build/python/build_ocean.py
# Build for a specific configuration
python build/python/build_ocean.py --config release

# Use a specific Visual Studio version
python build/python/build_ocean.py --vs-version 2022

# Specify custom directories
python build/python/build_ocean.py `
    --build-dir C:\build_ocean `
    --install-dir C:\install_ocean `
    --third-party-dir C:\ocean_3rdparty\install

# Show build plan without building
python build/python/build_ocean.py --dry-run
```

Once the build is complete, the compiled binaries can be found in `ocean_install/win_x86_64_static_debug` and `.../win_x86_64_static_release` (or with `arm64_` prefix on ARM64 systems).

Run `python build/python/build_ocean.py --help` to see all available options.

### Using CMake Directly

Alternatively, you can invoke CMake directly:

```powershell
cd \path\to\ocean

# Configure
cmake -S . -B build_win `
    -DCMAKE_BUILD_TYPE=Release `
    -DBUILD_SHARED_LIBS=OFF `
    -DOCEAN_THIRD_PARTY_ROOT=.\ocean_3rdparty\install

# Build and install
cmake --build build_win --target install
```

To use a specific Visual Studio version with CMake directly, add the `-G` flag:

```powershell
cmake -S . -B build_win -G "Visual Studio 16 2019" `
    -DCMAKE_BUILD_TYPE=Release `
    -DBUILD_SHARED_LIBS=OFF `
    -DOCEAN_THIRD_PARTY_ROOT=.\ocean_3rdparty\install
```

### Building with Visual Studio

To open the project in Visual Studio after configuration:

```powershell
# Configure the project
python build/python/build_ocean.py --configure-only

# Open in Visual Studio
start ocean_build\win_x86_64_static_release\ocean.sln
```

Then build and run the desired targets from within Visual Studio.
