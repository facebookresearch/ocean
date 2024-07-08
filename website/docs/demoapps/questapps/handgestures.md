---
title: Hand Gestures
sidebar_position: 7
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

[HandGestures](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/handgestures/quest/HandGestures.cpp#L16) is a lightweight app that utilizes hand tracking and individual hand gestures for interaction, providing a simple yet powerful example of interactive VR.

<img src={require('@site/static/img/docs/demoapps/questapps/handgestures.jpg').default} alt="Image: Screenshot of the Hand Gestures Quest app" width="600" className="center-image"/>

The "Hand Gestures" app is built on the VRNativeApplicationAdvanced class, which automatically provides hand tracking and visualization capabilities. Similar to previous apps, the [`onFramebufferInitialized()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/handgestures/quest/HandGestures.cpp#L22) event function is utilized to set up the main rendering objects.

In the [`onPreRender()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/handgestures/quest/HandGestures.cpp#L59) function, the app checks if a hand is performing a specific gesture. If a gesture is detected, a text appears behind each hand indicating the interaction being performed. Additionally, depending on the gesture, different geometries are rendered to visualize the information associated with that particular hand gesture.

Since the app relies on hand tracking, the hand tracking capability [*com.oculus.permission.HAND_TRACKING*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/handgestures/quest/AndroidManifest.xml#L12) is included in the app's manifest file to ensure proper functionality.

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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/handgestures/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>

## Features
 - **Supported Hand Gestures**: Includes pinching, pointing, grabbing, and more.
 - **Visualization Mode Alteration**: Demonstrates how to use a controller button to change visualization modes.


## Interaction Enhancement
 - **Gesture-Based Commands**: Offers a user-friendly way to interact with VR environments through natural hand movements.
