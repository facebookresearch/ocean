# Building for Linux

This document describes the process to build Ocean on Linux.

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* CMake 3.25 or higher is required (for CMake preset support)
* Currently some packages will have to be installed using the package manager of your distributions (example commands and package names given below are for Fedora and Ubuntu, but they should be similar for other distributions):
  ```
  # Fedora
  sudo dnf install libX11-devel-1.7.0-9.el9.x86_64 \
      gtk3-devel-3.24.31-2.el9.x86_64 \
      mesa-libGL-devel \
      libgudev-devel \
      xz-devel

  # Ubuntu
  sudo apt-get install libx11-dev libudev-dev liblzma-dev libgl1-mesa-dev libxcb-glx0-dev
  ```

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script. By default, this will build all third-party libraries in both debug and release configurations with static linking.

```
cd /path/to/ocean
./build/cmake/build_thirdparty_linuxunix.sh
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/3rdparty/linux/x64_static_debug` and `.../linux/x64_static_release` (or `arm64_static_*` on ARM64 systems).

The build script can be customized using command-line parameters. Use `--config` to specify build configurations, `--link` for linking type, `-b` for build directory, and `-i` for installation directory. For example:

```
cd /path/to/ocean
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_thirdparty_linuxunix.sh --help` to see all available options.

> **Note:** By default, the build scripts only display error messages. To see more detailed CMake output, use `--log-level STATUS` (for general progress information) or other levels like `VERBOSE` or `DEBUG`.


## 3 Building Ocean

Ocean uses CMake presets for build configuration. The easiest way to build all Ocean libraries and apps is by using the provided build script, [`build/cmake/build_ocean.sh`](build/cmake/build_ocean.sh). By default, it will look for third-party libraries in `bin/cmake/3rdparty` (the default output from the previous step).

```
cd /path/to/ocean
./build/cmake/build_ocean.sh
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/linux/x64_static_debug` and `.../linux/x64_static_release` (or `arm64_static_*` on ARM64 systems).

The build script can be customized using command-line parameters. For example:

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_ocean.sh --help` to see all available options.

### Using CMake Presets Directly

Alternatively, you can use CMake presets directly without the build script:

```bash
# List all available presets
cmake --list-presets

# Configure and build using a preset
cmake --preset linux-x64-static-release -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/linux/x64_static_release"
cmake --build --preset linux-x64-static-release --target install
```
