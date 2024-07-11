---
title: External Camera
sidebar_position: 13
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The [External Camera](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/media/externalcamera/quest/ExternalCameraApplication.cpp#L18) app expands the functionality of Meta's Quest headsets by allowing users to connect and access external USB cameras. When the app is launched, it displays a straightforward menu [listing all connected USB cameras](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/media/externalcamera/quest/ExternalCameraApplication.cpp#L304), enabling easy identification and selection of the desired device.

<img src={require('@site/static/img/docs/demoapps/questapps/externalcamera.jpg').default} alt="Image: Screenshot of the External Camera app" width="600" className="center-image"/>

Following the selection of a camera, the app seeks permission to access the external USB device, ensuring security and user consent. After permission is granted, users are presented with a menu to select their preferred stream configuration for the video feed, allowing customization based on specific requirements or preferences.

The final step in the setup process is the real-time display of the live camera stream within the Quest environment. This feature not only broadens the capabilities of the Quest headset but also offers numerous possibilities for integrating real-world visuals into VR experiences for monitoring, broadcasting, or interactive purposes.

Before a USB camera can be accessed on the Quest, users must grant permission. Ocean's [`OceanUSBManager`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/system/usb/android/OceanUSBManager.java#L36) handles the necessary logic on the Java side. Once the manager is [initialized](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/media/externalcamera/quest/ExternalCameraApplicationActivity.java#L30), the native code can request permission and subsequently access the USB video cameras. Ocean's Media abstraction layer manages permission and camera access on the native side. Users can then access a new live camera through the [`Media::Manager`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/media/externalcamera/quest/ExternalCameraApplication.cpp#L126), just as they would with any other media object on any other platform.

Additional functionalities include: After a stream has been selected, users can press the controller buttons B or Y to stop the video stream and start over with a new camera or stream. Additionally, by pressing the controller buttons A or X, users can toggle the position of the video stream between world-locked and head-locked. Furthermore, when using a USB hub, the app allows users to choose from several connected cameras.

## Building

<Tabs groupId="target-os" queryString>
<TabItem value="quest" label="Quest">
The following instructions are for macOS but can be easily converted to other host operating systems. First, ensure that the

* [Android setup](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#android-setup) and
* [Quest setup](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_meta_quest.md#quest-setup)

have been completed. And that the third-party libraries have been built according to [these instructions](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_meta_quest.md#2-building-the-third-party-libraries).

Then run the following steps:

```bash
# Define environment variables required for Android and Quest apps as per setup instructions above:
# ANDROID_HOME, ANDROID_NDK, ANDROID_NDK_VERSION, JAVA_HOME, and OVRPlatformSDK_ROOT

# Adjust this to your location of the third-party libraries
export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty"

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/media/externalcamera/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>

<TabItem value="android" label="Android">
The following instructions are for macOS but can be easily converted to other host operating systems. First, ensure that the
[Android setup](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#android-setup) has been completed.
And that the third-party libraries have been built according to
[these instructions](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_meta_quest.md#2-building-the-third-party-libraries).

Then run the following steps:

```bash
# Define environment variables required for Android and Quest apps as per setup instructions above:
# ANDROID_HOME, ANDROID_NDK, ANDROID_NDK_VERSION, and JAVA_HOME

# Adjust this to your location of the third-party libraries
export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty"

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/media/externalcamera/android

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>

## Features

  - **USB Camera Connectivity**: Allows users to connect and access external USB cameras directly through the Meta Quest headsets.
  - **Easy Camera Selection**: Features a user-friendly menu that lists all connected USB cameras for quick identification and selection.
  - **Permission Management**: Ensures security and user consent by requesting permission to access external USB devices before proceeding.
  - **Customizable Stream Configuration**: Offers a menu for users to select their preferred video feed settings, catering to individual requirements and preferences.
  - **Real-Time Video Streaming**: Displays the live camera stream within the Quest environment, enhancing the VR experience by integrating real-world visuals.
  - **Interactive Controls**: Users can stop the video stream and select a new camera or stream configuration using controller buttons B or Y. The position of the video stream can also be toggled between world-locked and head-locked with buttons A or X.
  - **Support for Multiple Cameras**: When using a USB hub, the app provides the functionality to choose from several connected cameras, expanding the user's options.

## Tested Cameras

The following is a list of webcams that have been confirmed to work with the Quest devices.
This list represents a random sample of webcams available on the market and is **not** intended as a recommendation of any specific brand or model.
The webcams support at least one of the required stream types: Uncompressed ([FORMAT_YUYV16](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L560C4-L560C17) aka YUV2), Motion JPEG, or Frame-based (H.264).

  - Ailipu, HD USB Camera
  - Ailipu/ELP, 3D USB Camera (ELP-USB960P2CAM-V90)
  - Anker, PowerConf C200
  - DSA, Groov-e USB Camera
  - Logitech, BRIO 301
  - Logitech, C270 HD WEBCAM
  - Logitech, HD Pro Webcam C920
  - Sonix, USB 2.0 Camera

Please note that while these models have been tested, performance can vary based on specific setups and usage conditions.
Further, Compatibility may change at any time due to updates or changes in software or hardware.
