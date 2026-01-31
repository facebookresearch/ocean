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

* CMake 3.25 or higher is required (for CMake preset support)

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

**Ninja build tool (Windows only)**

* Install via Android Studio's CMake component (see Android NDK installation).
* Alternatively, ensure the Ninja executable is present in the directory tree pointed to by the `ANDROID_HOME` environment variable.

**Java Development Kit (JDK)**

* Recommended: [OpenJDK 22](https://jdk.java.net/22/)

### Environment Variables

Set the following environment variables to configure the Android build environment:

#### Linux/macOS

Add these lines to your shell configuration file (`.zshrc`, `.bashrc`, or `.profile`) or run them in your current terminal session:

```bash
# Android SDK location
export ANDROID_HOME="${HOME}/Library/Android/sdk"

# Android NDK location and version
export ANDROID_NDK="${HOME}/Library/Android/sdk/ndk/<NDK_VERSION>"
export ANDROID_NDK_VERSION="<NDK_VERSION>"

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
   - `ANDROID_NDK`: Path to Android NDK (e.g., `C:\Users\%USERNAME%\AppData\Local\Android\Sdk\ndk\<NDK_VERSION>`)
   - `ANDROID_NDK_VERSION`: NDK version number (e.g., `26.2.11394342`)
   - `JAVA_HOME`: Path to JDK (e.g., `C:\Program Files\Java\jdk-22`)
4. Click "OK" to save and restart your terminal or IDE

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script. On Linux/macOS, use [`build/cmake/build_thirdparty_android.sh`](build/cmake/build_thirdparty_android.sh). On Windows, use the PowerShell script [`build/cmake/build_thirdparty_android.ps1`](build/cmake/build_thirdparty_android.ps1). By default, this will build all third-party libraries in both debug and release configurations with static linking for the `arm64-v8a` ABI.

### Linux/macOS

```bash
cd /path/to/ocean
./build/cmake/build_thirdparty_android.sh
```

### Windows (PowerShell)

```powershell
cd \path\to\ocean
.\build\cmake\build_thirdparty_android.ps1
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/3rdparty/android/arm64-v8a_static_debug` and `.../android/arm64-v8a_static_release`.

The build script can be customized using command-line parameters. Use `-Config` (or `--config` on bash) to specify build configurations, `-Link` (or `--link`) for linking type, `-ABI` (or `--abi`) for Android ABI, `-Build` (or `-b`) for build directory, and `-Install` (or `-i`) for installation directory. For example:

### Linux/macOS

```bash
cd /path/to/ocean
./build/cmake/build_thirdparty_android.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty" --abi arm64-v8a
```

### Windows (PowerShell)

```powershell
cd \path\to\ocean
.\build\cmake\build_thirdparty_android.ps1 -Config debug,release -Link static -Build C:\build_ocean_thirdparty -Install C:\install_ocean_thirdparty -ABI arm64-v8a
```

To build for multiple ABIs:

```powershell
.\build\cmake\build_thirdparty_android.ps1 -ABI "arm64-v8a,armeabi-v7a,x86_64"
```

Run `./build/cmake/build_thirdparty_android.sh --help` (or `Get-Help .\build\cmake\build_thirdparty_android.ps1 -Detailed` on Windows) to see all available options.

> **Note:** By default, the build scripts only display error messages. To see more detailed CMake output, use `-LogLevel STATUS` (or `--log-level STATUS` on bash) for general progress information, or other levels like `VERBOSE` or `DEBUG`.


## 3 Using Ocean in external Android projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Android project. This assumes that the third-party libraries have been built as described above for the required Android ABIs.

Ocean uses CMake presets for build configuration. The unified build script [`build/cmake/build_ocean.sh`](build/cmake/build_ocean.sh) supports cross-compilation for Android from macOS, Linux, or Windows (via Git Bash). By default, the script will look for third-party libraries in `bin/cmake/3rdparty` (the default output from the previous step).

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -p android
```

Once the build is complete, the compiled binaries can be found in `bin/cmake/android/arm64_static_release` (or with `_debug` suffix for debug builds).

The build script can be customized using command-line parameters. For example, to build for multiple ABIs:

```
cd /path/to/ocean
./build/cmake/build_ocean.sh -p android -a arm64,arm32,x64 -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"
```

### Windows Users

On Windows, you can use either:

1. **PowerShell** (recommended):
   ```powershell
   cd \path\to\ocean
   .\build\cmake\build_ocean.ps1 -Platform android -Arch arm64
   ```

2. **Git Bash**:
   ```bash
   cd /path/to/ocean
   ./build/cmake/build_ocean.sh -p android
   ```

Run `./build/cmake/build_ocean.sh --help` or `.\build\cmake\build_ocean.ps1 -?` to see all available options.

### Using CMake Presets Directly

Alternatively, you can use CMake presets directly without the build script:

```bash
# List all available presets
cmake --list-presets

# Configure and build using a preset
cmake --preset android-arm64-static-release -DCMAKE_PREFIX_PATH="${HOME}/install_ocean_thirdparty/android/arm64_static_release"
cmake --build --preset android-arm64-static-release --target install
```

For projects that use Gradle as their main build system, they can take advantage of `externalNativeBuild` to build Ocean directly by adding something similar to the following to their configuration:

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

The APK (Android Package Kit) files generated by Ocean's test/demo Gradle configuration files can support four different Android ABIs (armeabi-v7a, arm64-v8a, x86, x86_64), allowing the Android package to run on devices with corresponding CPU architectures.  But depending on the project, it may be desirable to limit the set of ABIs supported.  Doing so reduces APK file size, reduces resources necessary for building third-party libraries, and reduces resources spent on building the Android package.

By default, the Gradle configs only enable the Android ABI `arm64-v8a`. The is support for `armeabi-v7a`, `x86`, and `x86_64` as
well, but those ABIs need to be enabled explicitly. To do that check the files `.../apps/build.gradle.kts` for the following
lines and change them as required.

```
# ndk { abiFilters.addAll(listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64" )) }
ndk { abiFilters.addAll(listOf("arm64-v8a")) }
```

To build the APK, run "gradlew" from the directory in the manner examplified below. Gradle will build the components of Ocean needed by the application being built in a temporary build directory. When building on Windows, for path length reasons, it should be placed close to the root of a filesystem. The default location on Windows is C:\tmp\ocean\gradle, on other build platforms the default build location is /tmp/ocean/gradle. This default can be overridden by either setting the environment variable OCEAN_GRADLE_BUILD_PATH or giving gradle the command line option -PoceanGradleBuildPath=${GRADLE_BUILD_PATH}.

```
# Adjust this to your location of the third-party libraries
export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty_android

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
