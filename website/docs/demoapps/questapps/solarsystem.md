---
title: Solar System
sidebar_position: 11
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The [SolarSystem](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/solarsystem/quest/SolarSystem.cpp#L15) app transports users into a beautifully rendered VR representation of our solar system. Using X3D for scene description, this app creates a rich, educational experience where users can explore animated planets and listen to an informative audio guide.

<img src={require('@site/static/img/docs/demoapps/questapps/solarsystem.jpg').default} alt="Image: Screenshot of the Solar System Quest app" width="600" className="center-image"/>

The majority of the application logic is implemented not in C++ but within a [X3D](https://github.com/facebookresearch/ocean/blob/v1.0.0/res/application/ocean/demo/platform/meta/quest/openxr/solarsystem/quest/assets/scene/solarsystem.x3dv#L1) scene file. [X3D](https://www.web3d.org/x3d/what-x3d/), an extension of VRML 97, is designed to facilitate simple VR experiences that can be specified in a text file. X3D allows for the specification of both the appearance and behavior of an experience, which can be accomplished without in-depth programming knowledge. It enables the definition of hierarchical transformations, animations, and simple interactions based on input sensors or audio outputs.

Once the rendering engine of the demo app is initialized, the X3D file is [loaded and positioned](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/solarsystem/quest/SolarSystem.cpp#L58) in relation to the user's coordinate system to ensure that, for example, the sun is displayed at a certain distance in front of the user.

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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/solarsystem/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>

## Features
 - **Rich VR Experience**: Users are teleported into an animated solar system, complete with detailed planetary models.
 - **Scene Description with X3D**: Utilizes X3D to create a comprehensive scene description, allowing for detailed and accurate representations of celestial bodies.
 - **Educational Audio Playback**: An audio guide provides insights and facts about each planet, enhancing the educational value of the experience.


## Educational and Immersive
 - **Interactive Learning**: Offers an engaging way to learn about the solar system, making it suitable for educational settings or for anyone interested in astronomy.
 - **Visual and Audio Engagement**: Combines visual animations with audio explanations to cater to different learning styles and increase retention of information.
