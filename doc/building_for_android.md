# Building for Android

This document describes the process to build Ocean for Android on Linux, macOS, or Windows. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Building Android apps that come with Ocean

## 1 Prerequisites

To build the project, you need to satisfy the following prerequisites:

### General build prerequisites

Please refer to the [main page](README.md) for general build prerequisites.

* Python 3.8 or higher

### Android Setup

**Install an Android SDK**

* Suggested Android API level: 32, 33, or 34. Other versions may work but have not been tested.
* Recommended installation via [Android Studio](https://developer.android.com/studio)
  1. Open Android Studio settings
  2. Navigate to "Languages & Frameworks" > "Android SDK" > "SDK Platforms"
  3. Select the desired SDK and press "Apply" to start the installation

**Install an Android NDK**

* Latest available stable version recommended.
* Installation via Android Studio:
  1. Open Android Studio settings
  2. Navigate to "Languages & Frameworks" > "Android SDK" > "SDK Tools"
  3. Select "NDK (Side by side)" and press "Apply" to start the installation

### Additional tools

**Java Development Kit (JDK)**

* Recommended: [OpenJDK 22](https://jdk.java.net/22/)

### Environment Variables

Set the following environment variables to configure the Android build environment:

#### Linux/macOS

Add these lines to your shell configuration file (`.zshrc`, `.bashrc`, or `.profile`) or run them in your current terminal session:

```bash
# Android SDK location
export ANDROID_HOME="${HOME}/Library/Android/sdk"

# Android NDK location (used by the build system for cross-compilation)
# Set ANDROID_NDK_HOME or ANDROID_NDK_ROOT to the NDK directory
export ANDROID_NDK_HOME="${HOME}/Library/Android/sdk/ndk/<NDK_VERSION>"

# Java Development Kit location
export JAVA_HOME="/Library/Java/JavaVirtualMachines/openjdk-22.0.1.jdk/Contents/Home"
```

**Note:** Replace `<NDK_VERSION>` with your installed NDK version (e.g., `26.2.11394342`). Adjust other paths according to your system. On Linux, `ANDROID_HOME` is typically `${HOME}/Android/Sdk`.

#### Windows

Configure the environment variables through System Properties:

1. Press `Windows + R`, type `sysdm.cpl`, and press Enter
2. Go to the "Advanced" tab and click "Environment Variables"
3. Under "System variables", add the following variables:
   - `ANDROID_HOME`: Path to Android SDK (e.g., `C:\Users\%USERNAME%\AppData\Local\Android\Sdk`)
   - `ANDROID_NDK_HOME`: Path to Android NDK (e.g., `C:\Users\%USERNAME%\AppData\Local\Android\Sdk\ndk\<NDK_VERSION>`)
   - `JAVA_HOME`: Path to JDK (e.g., `C:\Program Files\Java\jdk-22`)
4. Click "OK" to save and restart your terminal or IDE

## 2 Building the third-party libraries

The third-party libraries are built using the Python-based build system. The same script works on Linux, macOS, and Windows.

```bash
cd /path/to/ocean

# Build all required third-party libraries for Android (all ABIs, debug + release, static)
python build/python/build_ocean_3rdparty.py --target android

# Build for a specific Android architecture
python build/python/build_ocean_3rdparty.py --target android_arm64

# Build for multiple specific architectures
python build/python/build_ocean_3rdparty.py --target android_arm64,android_x86_64

# Build release only
python build/python/build_ocean_3rdparty.py --target android --config release

# Specify a custom Android API level
python build/python/build_ocean_3rdparty.py --target android --android-api-level 34

# Show build plan without building
python build/python/build_ocean_3rdparty.py --target android --dry-run
```

On Windows (PowerShell):

```powershell
cd \path\to\ocean
python build/python/build_ocean_3rdparty.py --target android
```

Once the build is complete, the installed libraries can be found in `ocean_3rdparty/install/`. Headers are stored in `<lib>/h/android/` and libraries in `<lib>/lib/android_arm64_static_release/` (and similar paths for other architectures and configurations).

Run `python build/python/build_ocean_3rdparty.py --help` to see all available options.

> **Note:** The build system displays a real-time TUI with progress for all parallel build jobs. Use `--log-level verbose` to see detailed build output instead.


## 3 Using Ocean in external Android projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Android project. This assumes that the third-party libraries have been built as described above for the required Android ABIs.

```bash
cd /path/to/ocean

# Build Ocean for Android
python build/python/build_ocean.py --target android_arm64 --third-party-layout python

# Build for multiple architectures
python build/python/build_ocean.py --target android_arm64,android_x86_64 --third-party-layout python

# Build for a specific configuration
python build/python/build_ocean.py --target android_arm64 --third-party-layout python --config release

# Specify custom directories
python build/python/build_ocean.py --target android_arm64 --third-party-layout python \
    --build-dir "${HOME}/build_ocean" \
    --install-dir "${HOME}/install_ocean" \
    --third-party-dir /path/to/ocean_3rdparty/install
```

Once the build is complete, the compiled binaries can be found in `ocean_install/android_arm64_static_release` (or with `_debug` suffix for debug builds).

Run `python build/python/build_ocean.py --help` to see all available options.

### Using CMake Directly

Alternatively, you can invoke CMake directly:

```bash
cd /path/to/ocean

# Configure and build
cmake -S . -B build_android \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DOCEAN_THIRD_PARTY_LAYOUT=python \
    -DOCEAN_THIRD_PARTY_ROOT=./ocean_3rdparty/install \
    -DANDROID_PLATFORM=android-32

cmake --build build_android --target install -j
```

Projects that use Gradle as their main build system can take advantage of `externalNativeBuild` to build Ocean directly by adding something similar to the following to their configuration:

```
externalNativeBuild {
  cmake {
    arguments += "-DBUILD_SHARED_LIBS=OFF"
    arguments +=
        "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
    targets += "application_ocean_demo_base_console_android_native"
  }
}
```

For a full example, please take a look at the Gradle configuration of the Ocean Android apps.  For example, [`build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts`](build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts).

For projects using build systems other than Gradle, the precise details of the integration of Ocean are beyond the scope of this document and are left to the reader.

## 4 Building the Ocean Android demo/test apps

First, build the required third-party libraries as described above for the required Android ABIs. Then find the Gradle configuration of an Ocean Android app that you want to build, for example [`build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts`](build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts).

The APK (Android Package Kit) files generated by Ocean's test/demo Gradle configuration files can support four different Android ABIs (armeabi-v7a, arm64-v8a, x86, x86_64), allowing the Android package to run on devices with corresponding CPU architectures. But depending on the project, it may be desirable to limit the set of ABIs supported. Doing so reduces APK file size, reduces resources necessary for building third-party libraries, and reduces resources spent on building the Android package.

By default, the Gradle configs only enable the Android ABI `arm64-v8a`. There is support for `armeabi-v7a`, `x86`, and `x86_64` as well, but those ABIs need to be enabled explicitly. To do that, check the files `.../apps/build.gradle.kts` for the following lines and change them as required.

```
# ndk { abiFilters.addAll(listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64" )) }
ndk { abiFilters.addAll(listOf("arm64-v8a")) }
```

To build the APK, run "gradlew" from the directory in the manner exemplified below. Gradle will build the components of Ocean needed by the application being built in a temporary build directory. When building on Windows, for path length reasons, it should be placed close to the root of a filesystem. The default location on Windows is C:\tmp\ocean\gradle, on other build platforms the default build location is /tmp/ocean/gradle. This default can be overridden by either setting the environment variable OCEAN_GRADLE_BUILD_PATH or giving gradle the command line option -PoceanGradleBuildPath=${GRADLE_BUILD_PATH}.

```
# Adjust this to your location of the third-party libraries
export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty_android"

cd /path/to/ocean/build/gradle/application/ocean/demo/base/console/android

# Build the APK of the application
./gradlew build

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```

Log messages can be displayed using:

```
adb logcat -s Ocean
```
