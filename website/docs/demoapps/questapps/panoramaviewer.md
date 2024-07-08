---
title: Panorama Viewer
sidebar_position: 8
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The [PanoramaViewer](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest/PanoramaViewer.cpp#L20) demo app demonstrates how a 360-degree image can be utilized to create a teleportation experience in VR. This app introduces the use of visual assets like pictures and offers insights into adjusting properties within the rendering engine.

<img src={require('@site/static/img/docs/demoapps/questapps/panoramaviewer.jpg').default} alt="Image: Screenshot of Panorama Viewer Quest app" width="600" className="center-image"/>

The demo creates a large-radius rendering sphere object in the [`createTexturedSphere()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest/PanoramaViewer.cpp#L107) function. A 360-degree texture is then applied from the inside. By default, the sphere (and texture) would not be visible from the inside due to default geometry visibility settings. This visibility issue is easily rectified by appending a [**PrimitiveAttribute**](https://www.internalfb.com/intern/staticdocs/ocean/doxygen/class_ocean_1_1_rendering_1_1_primitive_attribute.html) to the sphere's attribute set.

Before the texture can be utilized, it must be transferred from the application's asset container to a temporary directory associated with the application. This transfer is facilitated by Ocean's [**ResourceManager**](https://www.internalfb.com/intern/staticdocs/ocean/doxygen/class_ocean_1_1_platform_1_1_android_1_1_resource_manager.html) for Android.

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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>

## Features
 - **360-Degree Image Utilization**: Uses a panoramic image to create immersive VR experiences.
 - **Asset Introduction**: Introduces the use of external assets like images in VR applications.


## Application Potential
 - **Tourism and Real Estate**: Ideal for applications in tourism, real estate, and other fields where immersive visualization is beneficial.
