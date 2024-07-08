---
title: Theremin
sidebar_position: 10
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The [Theremin app](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.cpp#L17) simulates a [Theremin](https://en.wikipedia.org/wiki/Theremin) musical instrument in VR, offering an interactive and immersive musical experience. Users can control the pitch and volume of sounds generated in real-time using hand movements, while the app also integrates passthrough technology for a mixed reality experience.

<img src={require('@site/static/img/docs/demoapps/questapps/theremin.jpg').default} alt="Image: Screenshot of the Theremin Quest app" width="600" className="center-image"/>

The implementation of the Theremin experience is straightforward. A simple brown-colored box represents the body of the instrument, while two gray cylinders act as antennas. A more detailed 3D model could be loaded from a scene file, such as glTF or OBJ, but we have opted to keep it simple in this instance.

In the [`onPreRender()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.cpp#L107) function, the distances between the hand's palm and the antennas are converted into frequency and volume parameters. The audio is then generated as PCM (Pulse-Code Modulation) in the [`renderAudioSample()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.cpp#L202) function and forwarded to a speaker object obtained from Ocean's [Media library](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.cpp#L20).

Furthermore, the app utilizes passthrough to create a Mixed Reality experience (all previous demo apps were pure VR experiences). Activating passthrough requires just [one line of code](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.cpp#L113) and the registration of the [*com.oculus.feature.PASSTHROUGH*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/platform/meta/quest/openxr/theremin/quest/AndroidManifest.xml#L13) capability in the manifest file.


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

cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/platform/meta/quest/openxr/theremin/quest

./gradlew assemble

# Install debug build of the app
adb install app/build/outputs/apk/debug/app-debug.apk

# Install release build of the app
adb install app/build/outputs/apk/release/app-release.apk
```
</TabItem>
</Tabs>

## Features
 - **Theremin Simulation**: Simulates the operation of a Theremin, an electronic musical instrument controlled without physical contact.
 - **Real-Time Sound Generation**: Users interact with their hands to control the pitch and volume, creating sounds in real-time.
 - **Passthrough Activation**: Incorporates passthrough technology, allowing users to see their real environment while interacting with the virtual Theremin.


## Interaction and Sound
- **Hand-Based Sound Control**: Provides a unique way to interact with sound, using hand movements to influence musical outputs.
- **Educational and Entertaining**: Ideal for educational purposes to demonstrate the principles of sound or simply as an entertaining VR experience.
