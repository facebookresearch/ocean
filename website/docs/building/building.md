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
- **CMake 3.26 or higher**: Build system generator
- A compatible **C++ compiler** for your target platform

## Getting the Source Code

Clone the Ocean repository:

```bash
git clone https://github.com/facebookresearch/ocean.git
cd ocean
```

Set the environment variable `OCEAN_DEVELOPMENT_PATH` to the location of the cloned repository. This variable is used by the build scripts:

```bash
export OCEAN_DEVELOPMENT_PATH="${HOME}/ocean"
```

## Build Overview

Ocean's build process generally follows these steps:

1. **Build third-party dependencies** - Ocean relies on several third-party libraries that need to be built first
2. **Build Ocean** - Build the Ocean libraries and applications using the provided build scripts

Each platform has specific requirements and build scripts. See the platform-specific sub-pages for detailed instructions:

- [Android](./building_for_android.md)
- [iOS](./building_for_ios.md)
- [Linux](./building_for_linux.md)
- [macOS](./building_for_macos.md)
- [Meta Quest](./building_for_meta_quest.md)
- [Windows](./building_for_windows.md)

:::tip
This documentation is also available on [GitHub](https://github.com/facebookresearch/ocean?tab=readme-ov-file#getting-started).
:::
