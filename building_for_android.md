# Building for Android

This document describes the process to build Ocean for Android. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Building Android apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Install an Android SDK (Android API 34).  Other versions may work but have not been tested.
  * Optionally, use [Android Studio](https://developer.android.com/studio) to install
    * Open Android Studio's Settings window.  Then navigate to "Languages & Frameworks" and "Android SDK" subsection.  Then from the "SDK Platforms" tab, select the SDK with the appropriate API level.  "Apply" the change.
* Install Android NDK (latest available stable version)
  * Use Android Studio to install "NDK (Side by side)".  Installing Android NDK by other meand may work, but that has not been tested with Ocean build configuration.
    * Open Android Studio's Settings window.  Then navigate to "Languages & Frameworks" and "Android SDK" subsection.  Then from the "SDK Tools" tab, select "NDK (Side by side)".  "Apply" the change.
* Install the ninja build tool and ensure its executable is present within the directory tree pointed to by the `ANDROID_HOME` environment variable (see below). An easy way to do this is to install the CMake component available under the "SDK Tools" tab on the "Android SDK" page of Android Studio's Settings facility.
* Install a Java Development Kit (JDK), for example [OpenJDK 22](https://jdk.java.net/22/).
* Define the following environment variables:
  * `ANDROID_HOME` - points to the location of the Android SDK, for example: `${HOME}/Library/Android/sdk` (on macOS)
  * `ANDROID_NDK` - points to the location of the Android NDK, for example: `${HOME}/Library/Android/sdk/ndk/26.2.11394342` (on macOS)
  * `ANDROID_NDK_VERSION` - Android NDK version number, for example: `26.2.11394342`
    * May be part of `ANDROID_NDK` path or found as "Pkg.BaseRevision" property listed in `${ANDROID_NDK}/source.properties`
  * `JAVA_HOME` - points to the location of the JDK, for example: `/Library/Java/JavaVirtualMachines/openjdk-22.0.1.jdk/Contents/Home` (on macOS)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build scripts, [`build/cmake/build_thirdparty_android.sh`](build/cmake/build_thirdparty_android.sh) (on Windows: [`build/cmake/build_thirdparty_android.bat`](build/cmake/build_thirdparty_android.bat)). Simply comment out all build configurations that are not required for your project.  See below for an example configuration that is sufficient for building debug and release build of Android packages if targeting 64-bit ("arm64-v8a" ABI) and 32-bit ("armeabi-v7a" ABI) ARM hardware:

```
run_build_for_android armeabi-v7a android-32 Debug static
run_build_for_android arm64-v8a   android-32 Debug static
# run_build_for_android x86         android-32 Debug static
# run_build_for_android x86_64      android-32 Debug static

# run_build_for_android armeabi-v7a android-32 Debug shared
# run_build_for_android arm64-v8a   android-32 Debug shared
# run_build_for_android x86         android-32 Debug shared
# run_build_for_android x86_64      android-32 Debug shared

run_build_for_android armeabi-v7a android-32 Release static
run_build_for_android arm64-v8a   android-32 Release static
# run_build_for_android x86         android-32 Release static
# run_build_for_android x86_64      android-32 Release static

# run_build_for_android armeabi-v7a android-32 Release shared
# run_build_for_android arm64-v8a   android-32 Release shared
# run_build_for_android x86         android-32 Release shared
# run_build_for_android x86_64      android-32 Release shared
```

Once the script completes, all binaries and include files of the third-party libraries will have been installed into `/tmp/ocean/install/android/${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_TYPE}`. On a Windows build host, they will be installed into that directory under drive C: with "android" abbreviated to "and" due to path name length considerations.

## 3 Using Ocean in external Android projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Android project.

First, build the required third-party libraries as described above for the required Android ABIs.

An external project that use Gradle as their main build system can take advantage of `externalNativeBuild` to build Ocean directly by adding something similar to the following to their configuration:

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

To limit the set of ABIs supported , edit the Gradle configuration file and comment out or remove the unused ABIs from Android NDK configuration.  See below for an example configuration that builds APK for "arm64-v8a" and "armeabi-v7a" while omitting "x86" and "x86_64".

```
ndk { abiFilters.addAll(listOf("arm64-v8a", "armeabi-v7a" /*, "x86", "x86_64"*/ )) }
```

To build the APK, run "gradlew" from the directory in the manner examplified below.

```
cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/base/console/android

# Build the APK of the application
./gradlew build -PoceanThirdPartyPath=/tmp/ocean/install/android

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
