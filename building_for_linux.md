# Building for Linux

This document describes the process to build Ocean on Linux.

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
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

Once the build is complete, the compiled binaries can be found in `ocean_install_thirdparty/linux/x64_static_debug` and `.../linux/x64_static_release` (or `arm64_static_*` on ARM64 systems).

The build script can be customized using command-line parameters. Use `--config` to specify build configurations, `--link` for linking type, `-b` for build directory, and `-i` for installation directory. For example:

```
cd /path/to/ocean
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_thirdparty_linuxunix.sh --help` to see all available options.


## 3 Building Ocean

The easiest way to build all Ocean libraries and apps is by using the provided build script. By default, it will look for third-party libraries in `ocean_install_thirdparty` (the default output from the previous step).

```
cd /path/to/ocean
./build/cmake/build_ocean_linuxunix.sh
```

Once the build is complete, the compiled binaries can be found in `ocean_install/linux/x64_static_debug` and `.../linux/x64_static_release` (or `arm64_static_*` on ARM64 systems).

The build script can be customized using command-line parameters. For example:

```
cd /path/to/ocean
./build/cmake/build_ocean_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

Run `./build/cmake/build_ocean_linuxunix.sh --help` to see all available options.
