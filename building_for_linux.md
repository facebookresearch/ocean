# Building for macOS

This document describes the process to build Ocean on Linux.

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Currently the following packages will have to be installed using the package manager of your distributions (names below are for Fedora, but they should be similar for other distributions):
  ```
  sudo dnf install libX11-devel-1.7.0-9.el9.x86_64 \
      gtk3-devel-3.24.31-2.el9.x86_64 \
      mesa-libGL-devel \
      libgudev-devel \
      xz-devel
  ```

## 2 Building the third-party libraries

To build the required third-party libraries for Ocean, run the following:

```
cd ${OCEAN_DEVELOPMENT_PATH}
./build/cmake/build_thirdparty_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty_linux" -i "${HOME}/install_ocean_thirdparty_linux"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Once the build is complete, the compiled binaries can be found in `${HOME}/install_ocean_thirdparty_linux/static_Debug` and `.../static_Release`.


## 3 Building Ocean

The easiest way to build the third-party libraries is by using the provided build scripts. In a terminal, run:

```
cd ${OCEAN_DEVELOPMENT_PATH}
./build/cmake/build_ocean_linuxunix.sh -c debug,release -l static -b "${HOME}/build_ocean_linux" -t "${HOME}/install_ocean_thirdparty_linux"
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Make sure that the parameter specifying the location of the third-party libraries is the same as installation path from the previous section. Once the build is complete, the compiled binaries can be found in `${HOME}/install_ocean_linux/static_Debug` and `.../static_Release`.
