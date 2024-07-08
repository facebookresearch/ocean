---
title: Finger Distance
sidebar_position: 6
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

[FingerDistance](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest/FingerDistance.cpp), based on [VRNativeApplicationAdvanced](vrnativeapplicationadvanced.md), measures the distance between both index fingers and visualizes this distance in VR. This app is an excellent introduction to hand interaction and the visualization of simple 3D objects.

<img src={require('@site/static/img/docs/demoapps/questapps/fingerdistance.png').default} alt="Image: Screenshot of the Finger Distance app" width="600" className="center-image"/>

The demo app initializes the necessary rendering objects after the framebuffer has been set up in the [`onFramebufferInitialized()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest/FingerDistance.cpp#L22) function. It creates a small sphere object for the tip of each index finger, a cylinder to indicate the connection between the two fingers, and a text object to display the distance between the fingertips.

Before rendering each new frame, the [`onPreRender()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest/FingerDistance.cpp#L67) event function is called. This function serves as the ideal location to modify the appearance of objects and to implement the application logic of the experience. The demo app accesses the poses of both hands and extracts the 3D positions of the tips of the index fingers. If the two fingers are tracked, the code updates the positions of the spheres and calculates a 6-DOF transformation for the cylinder and the text object. This ensures that the cylinder visually connects both fingers and that the text is always oriented towards the user.

As the app relies on hand tracking, the hand tracking capability [*com.oculus.permission.HAND_TRACKING*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest/AndroidManifest.xml#L12) is included in the app's manifest file. Without this capability configured, hand tracking will not function, and the app will be unable to access hand poses.


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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>


## Features
 - **Distance Measurement and Visualization**: Measures and visually represents the distance between index fingers in VR.
 - **Simple 3D Object Visualization**: Demonstrates visualization of spheres, cylinders, and text.


## Use case
- **Educational Tool**: Serves as a practical example for developers interested in creating educational or training applications focused on hand interactions.
