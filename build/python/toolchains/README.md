# CMake Toolchains

This directory contains CMake toolchain files for cross-compilation.

## ios.toolchain.cmake

iOS toolchain from [leetal/ios-cmake](https://github.com/leetal/ios-cmake) v4.4.1.

Licensed under BSD-3-Clause license.

### Usage

The toolchain is automatically used by the build system when targeting iOS.
Key variables:
- `PLATFORM`: Target platform (OS64 for device, SIMULATOR64 for simulator)
- `DEPLOYMENT_TARGET`: Minimum iOS version

### Updating

To update to a newer version:
1. Download from https://github.com/leetal/ios-cmake
2. Replace this file
3. Update version in this README
