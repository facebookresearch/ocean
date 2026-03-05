---
title: Building
sidebar_position: 1
description: Building Ocean for different platforms
---

# Building Ocean

This document describes the general process to build Ocean. Platform-specific instructions are available in the sub-pages.

## Prerequisites

Before building Ocean, ensure you have the following:

- **Git**: Required to clone the repository
- **Python 3.8 or higher**: Required for the build scripts
- **CMake 3.26 or higher**: Build system generator
- A compatible **C++ compiler** for your target platform

## Getting the Source Code

Clone the Ocean repository:

```bash
git clone https://github.com/facebookresearch/ocean.git
cd ocean
```

## Build Overview

Ocean's build process follows two steps:

1. **Build third-party dependencies** using `build/python/build_ocean_3rdparty.py` — this fetches, patches, and builds all required libraries with DAG-based parallel builds
2. **Build Ocean** using `build/python/build_ocean.py` — this configures and builds Ocean via CMake with the correct settings for your target platform

### Quick Start (macOS/Linux)

```bash
# Build third-party libraries
python build/python/build_ocean_3rdparty.py

# Build Ocean
python build/python/build_ocean.py --third-party-layout python
```

### Quick Start (Windows)

```powershell
# Build third-party libraries
python build/python/build_ocean_3rdparty.py

# Build Ocean
python build/python/build_ocean.py --third-party-layout python
```

The build system supports cross-compilation for multiple platforms from a single host. For example, on macOS you can build for iOS and Android in addition to macOS itself.

Each platform has specific requirements and options. See the platform-specific sub-pages for detailed instructions:

- [Android](./building_for_android.md)
- [iOS](./building_for_ios.md)
- [Linux](./building_for_linux.md)
- [macOS](./building_for_macos.md)
- [Meta Quest](./building_for_meta_quest.md)
- [Windows](./building_for_windows.md)

:::tip
This documentation is also available on [GitHub](https://github.com/facebookresearch/ocean?tab=readme-ov-file#getting-started).
:::
