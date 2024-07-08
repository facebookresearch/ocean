# Building for Meta Quest

This document describes the process to build Ocean projects for Meta Quest devices. It covers:

1. General requirements
2. Building required third-party libraries
3. Building Quest demo apps that come with Ocean

## 1 Prerequisites

To build the project, you need to satisfy the following prerequisites:

### General build prerequisites

Please refer to the [main page](README.md) for general build prerequisites.

### Android Setup

Please refer to the [instructions to build Ocean of Android](building_for_android.md#1-prerequisites) for the Android setup.

### Quest setup

**Activate Developer Mode (Required)**

To install, run, and debug native apps on your Quest device, it is essential to enable Developer Mode first.
This mode allows you to directly communicate with your device, essential for development and testing purposes.

Please follow the detailed guide provided by Oculus to enable Developer Mode on your Quest: [Mobile Device Setup](https://developer.oculus.com/documentation/native/android/mobile-device-setup/).
Once your Quest is in Developer Mode, you can connect it to your computer via a USB cable.
To verify that your device is properly set up and recognized, use the following adb command:

```
adb devices
```

This command should list your Quest device, indicating that it is ready for development activities.
If your device does not appear, ensure that your USB drivers are correctly installed and that the USB cable is functioning properly.

**Oculus (OVR) Platform SDK (Optional)**

While this SDK is optional some of the Ocean demo apps for Quest require this setup, for example Microphone demo for Quest devices.
* Download the most recent SDK: [Oculus Platform SDK](https://developer.oculus.com/downloads/package/oculus-platform-sdk/)
* Uncompress the archive and move the uncompressed directory to its destination folder
* Define the following environment variable:
    * `OVRPlatformSDK_ROOT` - points to root directory of unzipped Oculus Platform SDK files, e.g. `${HOME}/Downloads/ovr_platform_sdk_XX.Y` where `XX.Y` needs to be replaced with the version that was downloaded.
* When setup properly, following files should exist in the directory structure located at `${OVRPlatformSDK_ROOT}`:
    * `${OVRPlatformSDK_ROOT}/Android/libs/arm64-v8a/libovrplatformloader.so`
    * `${OVRPlatformSDK_ROOT}/Include/OVR_Platform.h` (and other header files in the same directory)

## 2 Building the third-party libraries

Please refer to the builds steps in the [instructions for Android](building_for_android.md#2-building-the-third-party-libraries) for details about building the required third-party libraries. For Quest only the following build parameters are required:

* Android ABI: `arm64-v8a`
* Android SDK: `android-32`
* Linking type: `static`
* Build config: `debug` and/or `release`

The following is a possible example build command:

```
cd ${OCEAN_DEVELOPMENT_PATH}

./build/cmake/build_thirdparty_android.sh -c debug,release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty" --abi arm64-v8a --sdk android-32
```

Once the script completes, all binaries and include files of the third-party libraries will have been installed into `${HOME}/install_ocean_thirdparty/android_${ANDROID_ABI}_${LINKING_TYPE}_${BUILD_TYPE}`.

**Note:** on a Windows build host, the binaries and include files the third-party libraries will be installed into `C:\and\...`.  This is required due to Windows imposing a maximum path length. Exceeding this limit will result in build errors, for example errors about missing files.
## 3 Building Quest demo apps that come with Ocean

Please refer to the builds steps in the [instructions for Android](building_for_android.md#4-building-the-ocean-android-demo-test-apps) for details about building Ocean Android apps with Gradle.

The Gradle build configurations for Quest demo apps can be found under the directory structure at [`build/gradle/application/ocean/demo/platform/meta/quest/openxr/`](build/gradle/application/ocean/demo/platform/meta/quest/openxr/).

To build Quest demo apps, first build the required third-party libraries as described above. Then find the Gradle configuration of a Quest app that you want to build, for example [`build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts`](build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/app/build.gradle.kts).

To build the APK, run "gradlew" from the directory in the manner examplified below.

```
# Adjust this to your location of the third-party libraries
export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty_android

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest

# Build the APK of the application
./gradlew build

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```

The don the device and start the app from the menu. Log messages can be displayed using:

```
adb logcat -s Ocean
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
