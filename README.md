# Ocean - A C++ Framework For Computer Vision (CV) And Augmented Reality (AR) Applications

## Contributing

Please read the [contribution guidelines](CONTRIBUTING.md) before submitting any code changes.

# Getting Started

## General Prerequisites

- Install [CMake](https://cmake.org/download/). The recommended version is 3.26 or higher.
- Define an environment variable `OCEAN_DEVELOPMENT_PATH` that points to the local directory containing this repository

## Android (built from Linux or macOS)

### Prequisites

- Install an Android SDK and NDK, e.g., using [Android Studio](https://developer.android.com/studio). The recommended SDK version is 32 or higher but lower versions may work as well.
- Install a [Java Development Kit](https://www.oracle.com/java/technologies/downloads/). The required JDK version is 11 or higher.
- Define environment variables `ANDROID_NDK` and `JAVA_HOME` pointing the installation locations above, for example for macOS this could be:
  ```
  export ANDROID_NDK=${HOME}/Library/Android/sdk/ndk/26.2.11394342
  export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-22.jdk/Contents/Home
  ```

### Building the Ocean libraries for Android

To build and install the Ocean libraries, for example to use them in another Android app, configure the build as follows:

```
export OCEAN_BUILD_DIRECTORY=/tmp/ocean_build
export OCEAN_INSTALL_DIRECTORY=/tmp/ocean_install

cmake -S${OCEAN_DEVELOPMENT_PATH} \
    -B${OCEAN_BUILD_DIRECTORY} \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-32 \
    -DCMAKE_ANDROID_STL=c++_static \
    -DCMAKE_ANDROID_NDK=${ANDROID_NDK} \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
    -DCMAKE_INSTALL_PREFIX=${OCEAN_INSTALL_DIRECTORY} \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_BUILD_THIRD_PARTY_LIBS=TRUE
```

Then start the actual build step using:

```
cmake --build ${OCEAN_BUILD_DIRECTORY} --target install -- -j16
```

If you are targeting multiple ABIs, check out the build script [`build/cmake/build_for_android.sh`](build/cmake/build_for_android.sh) which will help with the automation of the builds.

### Building the Ocean apps for Android

This section is currently work in progress.

## License

Ocean is released under the [MIT License](LICENSE).

We provide no pre-built binaries. To use it, you must build Ocean from source.
