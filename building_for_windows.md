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

The installation location will be shown in the logs of the build script and defaults to a directory `ocean_install_thirdparty` in the current directory. It is advisable to place build and install directories as close to the root of a filesystem as possible, due to Windows limitations on path lengths. Build directories in particular can be very deep.

Methods for specifying the build and install directories, in descending order of preference are:
1. Using `-build X:\build\path` and/or `-install X:\install\path`
2. Changing hard-coded defaults `DEFAULT_BUILD_PATH` and `DEFAULT_INSTALL_PATH` in the build script files

For example:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat -build C:\build_oceanTP -install C:\install_oceanTP
```

Otherwise the code will be built in `ocean_build_thirdparty` in the current directory. Once the build is finished, this build directory can be discarded.

Under the installation and build directories, there will be one subdirectory per build config.

## 3 Building Ocean

The provided build script [`build/cmake/build_ocean_windows.bat`](build/cmake/build_ocean_windows.bat) will build all Ocean libraries and apps. It supports the same command-line configuration options (where applicable) as the third-party library build script. If the third-party dependencies were installed somewhere other than `ocean_install_thirdparty` in the current directory the Ocean build script needs to be informed, using the `-third-party` option, where to find them. For example to build only the static debug configurations, run:

```
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_ocean_windows.bat -config debug -link static -third-party X:\path\to\third-party\install\dir
```

## 4 Building Ocean with Visual Studio Projects

As an alternative to building Ocean with CMake (Section 3), you can use the provided Visual Studio solution and project files located in `build\visual_studio\win\vc143`. This approach is useful for development and debugging as it provides better IDE integration.

### Prerequisites

Before opening the Visual Studio projects, you need to build and organize the third-party libraries in a specific directory structure that the Visual Studio projects expect.

### Step 1: Build Third-Party Libraries with Subdivision

Build the third-party libraries using the `-subdivide ON` option, which organizes libraries into separate subdirectories:

```batch
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\build_thirdparty_windows.bat -build C:\build_3p -install C:\install_3p -link static -subdivide ON -config debug
.\build\cmake\build_thirdparty_windows.bat -build C:\build_3p -install C:\install_3p -link static -subdivide ON -config release
```

**Note:** The `-subdivide ON` flag is important as it creates separate directories for each library, which the reorganization script requires.

### Step 2: Reorganize Third-Party Installation

The Visual Studio projects expect a specific directory structure with version-organized paths. Use the reorganization script to transform the CMake installation into this structure:

```batch
cd /D %OCEAN_DEVELOPMENT_PATH%
.\build\cmake\reorganize_thirdparty_install.bat -i C:\install_3p -o C:\ocean_3rdparty
```

This script will reorganize the libraries from the flat CMake structure (e.g., `windows_static_Debug\libname\*`) into a version-first structure (e.g., `libname\version\lib\win_x64_vc143_static_debug\*`).

### Step 3: Create Symlink to Third-Party Libraries

The Visual Studio projects reference third-party libraries via `$(OCEAN_DEVELOPMENT_PATH)\3rdparty`. Create a symbolic link to point to your reorganized third-party directory:

```batch
cd /D %OCEAN_DEVELOPMENT_PATH%
mklink /D .\3rdparty C:\ocean_3rdparty
```

**Important:** This command requires **Administrator privileges**. Run Command Prompt or PowerShell as Administrator before executing this command.

**Note:** This symlink only needs to be created once. If you rebuild third-party libraries, you don't need to recreate the symlink - just re-run the reorganization script to update the contents of `C:\ocean_3rdparty`.

### Step 4: Open and Build in Visual Studio

1. Start Visual Studio 2022
2. Open the solution file: `build\visual_studio\win\vc143\ocean.sln`
3. Select your desired configuration (e.g., "Static Debug | x64")
4. Build the solution or individual projects as needed

The Visual Studio projects will automatically find the third-party libraries via the `$(OCEAN_DEVELOPMENT_PATH)\3rdparty` path.

### Current Limitations

When using the Visual Studio projects with locally-built third-party libraries, be aware of these limitations:

* **OpenSSL:** Not currently included in the CMake third-party builds, so `ocean/io/Signature.{h,cpp}` and related SSL functionality will not be available.
* **Google V8:** The JavaScript engine is not included, so `ocean/interaction/javascript` projects will fail to build.
* **Path Length Issues:** Windows has a default path limit of 260 characters (MAX_PATH). Depending on your directory structure, you might exceed this limit, resulting in build errors. **Workarounds:**
  - Choose build directories near the filesystem root (e.g., `C:\build_3p` instead of `C:\Users\YourName\Documents\Projects\Ocean\build_3p`)
  - Enable long path support in Windows 10/11 (see Microsoft documentation)
  - Modify the Windows registry to increase the path limit (search for "Windows 10 enable long paths")
