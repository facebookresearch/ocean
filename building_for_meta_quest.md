# Building for Meta Quest

This document describes the process to build Ocean projects for Meta Quest devices. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external Quest projects
4. Building Quest demo apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* [Android build prerequisites](building_for_android.md#1-prerequisites)
* (Optional) Download and set up Oculus (OVR) Platform SDK
  * Note: while the SDK is optional, some of Ocean's Meta Quest projects (e.g. Microphone demo for Quest devices) require this setup.
  * [Download Oculus Platform SDK](https://developer.oculus.com/downloads/package/oculus-platform-sdk/)
  * Unzip the downloaded file (e.g. "ovr_platform_sdk_65.0.zip") to a directory
  * Define the following environment variable:
    * `OVRPlatformSDK_ROOT` - points to root directory of unzipped Oculus Platform SDK files, e.g. `${HOME}/Downloads/ovr_platform_sdk_65.0`
      * When setup properly, following files should exist in the directory structure located at `${OVRPlatformSDK_ROOT}`:
        * `${OVRPlatformSDK_ROOT}/Android/libs/arm64-v8a/libovrplatformloader.so`
        * `${OVRPlatformSDK_ROOT}/Include/OVR_Platform.h` (and other header files in the same directory)

## 2 Building the third-party libraries

The easiest way to build the third-party libraries is by using the provided build script, [`build/cmake/build_thirdparty_android.sh`](build/cmake/build_thirdparty_android.sh). Simply comment out all build configurations that are not required for your project.  Currently, Quest demo applications only build with "arm64-v8a" static library builds of Ocean.  See example below for configuration sufficient to build debug and release builds of Meta Quest apps:

```
# run_build_for_android armeabi-v7a android-32 Debug static
run_build_for_android arm64-v8a   android-32 Debug static
# run_build_for_android x86         android-32 Debug static
# run_build_for_android x86_64      android-32 Debug static

# run_build_for_android armeabi-v7a android-32 Debug shared
# run_build_for_android arm64-v8a   android-32 Debug shared
# run_build_for_android x86         android-32 Debug shared
# run_build_for_android x86_64      android-32 Debug shared

# run_build_for_android armeabi-v7a android-32 Release static
run_build_for_android arm64-v8a   android-32 Release static
# run_build_for_android x86         android-32 Release static
# run_build_for_android x86_64      android-32 Release static

# run_build_for_android armeabi-v7a android-32 Release shared
# run_build_for_android arm64-v8a   android-32 Release shared
# run_build_for_android x86         android-32 Release shared
# run_build_for_android x86_64      android-32 Release shared
```

Once the script completes, all binaries and include files of the third-party libraries will have been installed into `/tmp/ocean/install/android/${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_TYPE}`. On a Windows build host, they will be installed into that directory under drive C: with "android" abbreviated to "and" due to path name length considerations.

## 3 Using Ocean in external Quest projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Meta Quest project.

First, build the required third-party libraries as described above.

An external project that uses Gradle as their main build system can take advantage of `externalNativeBuild` to build Ocean directly by adding something similar to the following to their configuration:

```
externalNativeBuild {
  cmake {
    arguments += "-DBUILD_SHARED_LIBS=OFF"
    arguments +=
        "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
    targets += "application_ocean_demo_platform_meta_quest_openxr_renderer_quest_native"
  }
}
```

For a full example, please take a look at the Gradle configuration of the Ocean Meta Quest apps.  For example, [`build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts`](build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts).

For projects using build systems other than Gradle, the precise details of the integration of Ocean are beyond the scope of this document and are left to the reader.

## 4 Building Quest demo apps that come with Ocean

Gradle build configurations for Quest demo apps can be found under the directory structure at [`build/gradle/application/ocean/demo/platform/meta/quest/openxr/`](build/gradle/application/ocean/demo/platform/meta/quest/openxr/).

To build Quest demo atts, first build the required third-party libraries as described [above](#2-building-the-third-party-libraries). Then find the Gradle configuration of a Quest app that you want to build, for example [`build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts`](build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts).

To build the APK, run "gradlew" from the project's `quest/` subdirectory in the manner examplified below.

```
# Traverse to project's 'quest' subdirectory
cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest

# Build the APK of the application
./gradlew build -PoceanThirdPartyPath=/tmp/ocean/install/android

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```

## Additional Notes

Not all Android apps provided with Ocean have been tested on Quest devices.

Building an Android app for Quest devices may require editing the app's Gradle configuration to lower the minimum Android SDK version requirement.  Installation of apps may fail if `minSdk` value set for the app is higher than Android SDK version on the device (e.g. `minSdk = 32` to be able to run on Quest 3 with system update from 2023.06.30).

See example Gradle configuration snippet below:

```
defaultConfig {
  applicationId = "com.meta.ocean.app.demo.platform.android.pixelformats.android"
  minSdk = 32
```
