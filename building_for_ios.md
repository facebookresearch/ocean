# Building for iOS

This document describes the process of building Ocean for iOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Building iOS apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Create an [Apple Developer account](https://developer.apple.com/), if you haven't already
* Retrieve the team ID of the above developer account. This will be an alphanumeric identifier of the form `XXXXXXXXXX`. Here are instructions to
  * [find it on the Apple page](https://developer.apple.com/help/account/manage-your-team/locate-your-team-id) and to
  * [find it in your local keychain](https://stackoverflow.com/a/47732584)
* Install a recent version of [XCode](https://developer.apple.com/xcode/) (recommended version is XCode 15).
* Install all required resources to build iOS following these [instructions](https://developer.apple.com/documentation/safari-developer-tools/installing-xcode-and-simulators).
* This document assumes the use a physical iPhone with ARM64-based CPUs, not a simulator. Building for old iPhones should be possible but hasn't been tested. For details check the toolchain file, [build/cmake/ios-cmake/ios.toolchain.cmake](https://github.com/facebookresearch/ocean/blob/7eb6a04e60278185c3ebe469829f00fc7092b223/build/cmake/ios-cmake/ios.toolchain.cmake#L59-L60).

## 2 Building the third-party libraries

As with the desktop use case, this process consists of two steps:

1. Building the required third-party libraries
2. Building the Ocean libraries

The easiest way to build the third-party libraries is by using the provided build scripts. In a terminal, run:

```
cd ${OCEAN_DEVELOPMENT_PATH}

# Run the build script with the default installation path ...
./build/cmake/build_thirdparty_ios.sh

# ... or, optionally, provide an installation location.
./build/cmake/build_thirdparty_ios.sh /path/to/install/into
```

## 3 Using Ocean in external XCode projects

This section provides an example of how to build the Ocean libraries to integrate into an existing iOS project.

First, build the required third-party libraries as described above. Then, review the build script for iOS builds of Ocean, [`build/cmake/build_ocean_ios.sh`](3/build/cmake/build_ocean_ios.sh#L130-L134), and comment out all build configurations that are not required. Ensure the selection of enabled build configurations matches the one from the build of the third-party libraries.

By default, all binaries and include files of Ocean will be installed into `/tmp/ocean/install/ios/${LINKING_TYPE_${BUILD_TYPE}`. Alternatively, you can integrate Ocean into any external project or follow the instructions to [include the CMake project of Ocean in an XCode project](https://blog.tomtasche.at/2019/05/how-to-include-cmake-project-in-xcode.html).

## 4 Building the Ocean iOS demo/test apps

First, build the required third-party libraries as described above.

Then configure the CMake project of Occean:

```
cmake -S"${OCEAN_DEVELOPMENT_PATH}/ocean" \
    -B"/tmp/ocean/build/ios/static_Debug" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE="${OCEAN_DEVELOPMENT_PATH}/ocean/build/cmake/ios-cmake/ios.toolchain.cmake" \
    -DPLATFORM="OS64" \
    -DDEPLOYMENT_TARGET="13.0" \
    -DCMAKE_INSTALL_PREFIX="/tmp/ocean/install/ios/static_Debug" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
    -DBUILD_SHARED_LIBS="OFF"
```

where `-DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM` specifies the Apple team ID. Please make sure to replace `XXXXXXXXXX` with your team ID. Update other parameters as required, for example:

* `-B` - the build directory for Ocean
* `-DCMAKE_BUILD_TYPE` - the build type to use (tested values are `Debug` and `Release`)

Once the configuration is complete, open the generated the XCode project:

```
open /tmp/ocean/build/ios/static_Debug/ocean.xcodeproj
```

Search for the demo/test apps at the top of the XCode window. The Ocean targets for demos and tests follow the naming scheme `application_ocean_(test|demo)_..._ios`. Select an app and build and install as normal.
