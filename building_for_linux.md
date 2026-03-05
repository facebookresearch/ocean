# Building for Linux

This document describes the process to build Ocean on Linux.

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Python 3.8 or higher
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

The third-party libraries are built using the Python-based build system. It handles fetching, patching, and building all dependencies with DAG-based parallel builds.

```bash
cd /path/to/ocean

# Build all required third-party libraries for the host platform (debug + release, static)
python build/python/build_ocean_3rdparty.py

# Build release only
python build/python/build_ocean_3rdparty.py --config release

# Include optional libraries (e.g., OpenCV)
python build/python/build_ocean_3rdparty.py --with opencv

# Show build plan without building
python build/python/build_ocean_3rdparty.py --dry-run
```

Once the build is complete, the installed libraries can be found in `ocean_3rdparty/install/`. Headers are stored in `<lib>/h/<platform>/` and libraries in `<lib>/lib/<target>/` (e.g., `zlib/lib/linux_x86_64_static_release/`). On ARM64 systems, paths use `arm64` instead of `x86_64`.

Run `python build/python/build_ocean_3rdparty.py --help` to see all available options.

> **Note:** The build system displays a real-time TUI with progress for all parallel build jobs. Use `--log-level verbose` to see detailed build output instead.


## 3 Building Ocean

Ocean is built using a Python build script that invokes CMake with the correct configuration.

```bash
cd /path/to/ocean

# Build Ocean using the Python 3P layout
python build/python/build_ocean.py --third-party-layout python

# Build for a specific configuration
python build/python/build_ocean.py --third-party-layout python --config release

# Specify custom directories
python build/python/build_ocean.py --third-party-layout python \
    --build-dir "${HOME}/build_ocean" \
    --install-dir "${HOME}/install_ocean" \
    --third-party-dir /path/to/ocean_3rdparty/install

# Show build plan without building
python build/python/build_ocean.py --third-party-layout python --dry-run
```

Once the build is complete, the compiled binaries can be found in `ocean_install/linux_x86_64_static_debug` and `.../linux_x86_64_static_release` (or `linux_arm64_static_*` on ARM64 systems).

Run `python build/python/build_ocean.py --help` to see all available options.

### Using CMake Directly

Alternatively, you can invoke CMake directly:

```bash
cd /path/to/ocean

# Configure and build using CMake
cmake -S . -B build_linux \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_THIRD_PARTY_LAYOUT=python \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install

cmake --build build_linux --target install -j
```
