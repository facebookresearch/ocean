# Ocean
## A C++ Framework For Computer Vision (CV) And Augmented Reality (AR) Applications

[![Build Ocean on Linux (static)](https://github.com/facebookresearch/ocean/actions/workflows/build_linux.yml/badge.svg)](https://github.com/facebookresearch/ocean/actions/workflows/build_linux.yml)
[![Build and deploy website](https://github.com/facebookresearch/ocean/actions/workflows/build_and_deploy_website.yml/badge.svg)](https://github.com/facebookresearch/ocean/actions/workflows/build_and_deploy_website.yml)

# Contributing

Please read the [contribution guidelines](CONTRIBUTING.md) before submitting any code changes.

# Getting Started

## General Prerequisites

- Install [CMake](https://cmake.org/download/). The required version is 3.29.
- Define an environment variable `OCEAN_DEVELOPMENT_PATH` that points to the local directory containing this repository
- Ocean requires C++20
- Ensure that the [Git](https://www.git-scm.com/downloads) executable is available in your path.

## Building

Please refer to the platform-specific instructions when building Ocean:

* [Android](building_for_android.md)
* [iOS](building_for_ios.md)
* [macOS](building_for_macos.md)
* [Windows](building_for_windows.md)

# License

Ocean is released under the [MIT License](LICENSE).

We provide no pre-built binaries. To use it, you must build Ocean from source.
