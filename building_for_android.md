# Building for Android

This document describes the process to build Ocean for Android on Linux or macOS. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Building Android apps that come with Ocean

## 1 Prerequisites

To build the project, you need to satisfy the following prerequisites:

### General build prerequisites

Please refer to the [main page](README.md) for general build prerequisites.

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

Define the following environment variables:
* `ANDROID_HOME`: points to the Android SDK location (e.g., `${HOME}/Library/Android/sdk` on macOS).
* `ANDROID_NDK`: points to the Android NDK location (e.g., `${HOME}/Library/Android/sdk/ndk/26.2.11394342` on macOS).
* `ANDROID_NDK_VERSION`: the Android NDK version number (e.g., 26.2.11394342).
* `JAVA_HOME`: points to the JDK location (e.g., `/Library/Java/JavaVirtualMachines/openjdk-22.0.1.jdk/Contents/Home` on macOS).

**Setting Environment Variables**

On Linux, macOS (and other Unixes) add the following lines to your terminal config file (e.g., `.zshrc`, `.bashrc`, `.profile`) or copy-and-paste them directly into the current terminal (each time):

```bash
export ANDROID_HOME="${HOME}/Library/Android/sdk"
export ANDROID_NDK="${HOME}/Library/Android/sdk/ndk/26.2.11394342"
export ANDROID_NDK_VERSION="26.2.11394342"
export JAVA_HOME="/Library/Java/JavaVirtualMachines/openjdk-22.0.1.jdk/Contents/Home"
```

On Windows, define them manually:
* Press `WINDOWS + R`, type `sysdm.pl`, and click `OK`.
* Switch to the "Advanced" tab and select "Environment variables".
* Add the variables as "System variables" and ensure correct paths for your system.
* A restart may be required.

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build scripts, [`build/cmake/build_thirdparty_android.sh`](build/cmake/build_thirdparty_android.sh) (on Windows: [`build/cmake/build_thirdparty_android.bat`](build/cmake/build_thirdparty_android.bat)). Simply comment out all build configurations that are not required for your project.  See below for an example configuration that is sufficient for building debug and release build of Android packages if targeting 64-bit ("arm64-v8a" ABI) and 32-bit ("armeabi-v7a" ABI) ARM hardware:

```
cd ${OCEAN_DEVELOPMENT_PATH}

./build/cmake/build_thirdparty_android.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty" --abi arm64-v8a
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Once the build is complete, the compiled binaries can be found in `${HOME}/install_ocean_thirdparty_android/static_Debug` and `.../static_Release`.


## 3 Using Ocean in external Android projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Android project. This assumes that the third-party libraries have been built as described above for the required Android ABIs.

After that you have following options:

If you already have a complete setup for an Android project and just need the header files and compiles libraries of Ocean, you
can build it as follows:

```
cd ${OCEAN_DEVELOPMENT_PATH}

./build/cmake/build_ocean_android.sh -c debug,release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty" --abi arm64-v8a
```

Change the values for the build config (`-c`), the build directory (`-b`), and the installation directory (`-i`) as required. Make sure that the parameter specifying the location of the third-party libraries is the same as installation path from the previous section. Also as before, we're only building for the Android ABI, `arm64-v8a`. Change this as required. Once the build is complete, the compiled binaries can be found in `${HOME}/build_ocean_android/static_Debug` and `.../static_Release`.

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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/base/console/android

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
