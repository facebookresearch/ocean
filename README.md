# Ocean

Ocean is a C++ framework for Computer Vision (CV) And Augmented Reality (AR) Applications.

[![Build Ocean on Linux (static)](https://github.com/facebookresearch/ocean/actions/workflows/build_linux.yml/badge.svg)](https://github.com/facebookresearch/ocean/actions/workflows/build_linux.yml)
[![Build and deploy website](https://github.com/facebookresearch/ocean/actions/workflows/build_and_deploy_website.yml/badge.svg)](https://github.com/facebookresearch/ocean/actions/workflows/build_and_deploy_website.yml)

# Contributing

Please read the [contribution guidelines](CONTRIBUTING.md) before submitting any code changes.

# Getting Started

For a general overview, the [project website](https://facebookresearch.github.io/ocean/) is a good starting point. For instructions to build Ocean, continue to read below.

## General Prerequisites

- Install [CMake](https://cmake.org/download/) 3.26 or higher.
- Define an environment variable `OCEAN_DEVELOPMENT_PATH` that points to the local directory containing this repository
- Ocean requires C++20
- Ensure that the [Git](https://www.git-scm.com/downloads) executable is available in your path.

## Building

Please refer to the platform-specific instructions when building Ocean:

* [Android](building_for_android.md)
* [iOS](building_for_ios.md)
* [Linux](building_for_linux.md)
* [macOS](building_for_macos.md)
* [Meta Quest](building_for_meta_quest.md)
* [Windows](building_for_windows.md)

Note that we provide no pre-built binaries. To use Ocean, you must build it from source.

# License

Ocean is released under the [MIT License](LICENSE).

## License Exceptions

Note that all files in the directory [`res/ocean/.../controller`](res/ocean/platform/meta/quest/application/assets/ocean_meta_quest_application/controller/)
are licensed under the [Art Attribution License 1.0](res/ocean/platform/meta/quest/application/assets/ocean_meta_quest_application/controller/LICENSE).
Please review the individual files for specific licensing details.
