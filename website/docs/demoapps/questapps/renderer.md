---
title: Renderer
sidebar_position: 4
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The [Renderer](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/OpenXRRendererApplication.cpp#L13) app extends the [VRNativeApplication](vrnativeapplication.md) by demonstrating how to render a simple coordinate system into VR. This visual representation helps in understanding spatial orientation in VR environments.

<img src={require('@site/static/img/docs/demoapps/questapps/rendererapplication.png').default} alt="Image: Hierarchy of Ocean's Quest environments" width="600" className="center-image"/>

Within the [`onFramebufferInitialized()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/OpenXRRendererApplication.cpp#L26) function, a 3D coordinate system is established. The scene remains static, meaning its appearance does not change over time. Optionally, the app also demonstrates how to integrate a custom render call, for instance, utilizing a third-party rendering engine. This feature provides flexibility for developers looking to customize the rendering process according to specific requirements or to leverage advanced rendering technologies.

<img src={require('@site/static/img/docs/demoapps/questapps/renderer.jpg').default} alt="Image: Screenshot of the Renderer Quest app" width="600" className="center-image"/>

Use this app as a foundational template for developing lightweight VR applications with minimal dependencies. It offers a streamlined approach for those looking to create efficient and straightforward VR experiences.

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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/renderer/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>


## Features
- **Coordinate System Visualization**: Displays the Quest's world coordinate system with 1-meter long axes in front of the user. Each axis has an own color (red for x-axis, green for y-axis, blue for z-axis).
- **Custom OpenGL ES Engine Integration**: Demonstrates how a custom OpenGL ES engine could be integrated into the rendering pipeline (disabled by default).


## Usage
- **Immediate Visual Representation**: Unlike the previous apps, this app provides immediate visual feedback by displaying the coordinate system upon app start.
