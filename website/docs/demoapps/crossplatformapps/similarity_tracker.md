---
title: Similarity Tracker
sidebar_position: 7
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The similarity tracker demo application implements a tracker that determines a similarity transformation between consecutive video frames. This transformation is based on sparse feature points tracked between frames. Users can define a region of interest (bounding box) by selecting an area within the video frame. The tracker will then detect and track sparse feature points only within this region. The region's location, scale, and orientation are updated in each new video frame.

<div class="center-images">
  <img src={require('@site/static/img/docs/demoapps/crossplatformapps/similarity_tracker_android.jpg').default} alt="Image: The homography image alignment app on Android" height="600" className="center-image"/>
</div>


### Similarity Transformation

A similarity transformation has four degrees of freedom, comprising rotation, scale, and 2D translation (in the x- and y-directions) within the image domain. The 3x3 matrix representing the similarity transformation has the following layout:

```
| a  -b  tx |
| b   a  ty |
| 0   0   1 |
```

where the elements `a` and `b` contain rotation and scale, and `tx` and `ty` represent the translation.

### Application Components

The application primarily utilizes Ocean's SimilarityTracker class, which implements the described functionality. The SimilarityTrackerWrapper class adds additional functionality, including media access management, gyro measurement acquisition, and visualization of the region of interest. Note that the SimilarityTracker has been integrated into the RegionTracker, used in WorldTracking and the sticker pinning effect, but does not possess re-localization capabilities.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="android" label="Android" default>
    Ensure the [third-party libraries have been built for Android](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#2-building-the-third-party-libraries) for all of the required Android ABIs. Let's assume the base location for third-party libraries is `${HOME}/install_ocean_thirdparty`, i.e., the Android versions will be located in `${HOME}/install_ocean_thirdparty/android_${ANDROID_ABI}...`.

    ```bash
    # Define this so that Gradle (and CMake) can find the third-party libraries.
    export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty"

    # Change into the directory with the Gradle config of this project
    cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/tracking/similaritytracker/android

    # In ./app/build.gradle.kts, ensure that only those Android ABIs are enabled for which the corresponding third-party libraries have been built. Otherwise, your build will fail, cf. the [general build instructions](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#4-building-the-ocean-android-demotest-apps).

    # Build the debug and release APK of this
    ./gradlew assemble

    # Install the desired APK
    adb install app/build/outputs/apk/debug/app-debug.apk
    adb install app/build/outputs/apk/release/app-release.apk
    ```

    By default, the log output will be displayed on the screen of the phone as well as in the Android logs, which can be displayed using:

    ```bash
    adb logcat -s Ocean
    ```

  </TabItem>

  <TabItem value="ios" label="iOS">
    Ensure the [third-party libraries have been built for iOS](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#2-building-the-third-party-libraries). Then follow the general [setup for building iOS apps using XCode](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#4-building-the-ocean-ios-demotest-apps). Once the generation of the XCode project is complete, open it and search for `application_ocean_demo_tracking_similaritytracker_ios` in the scheme at the top. Then hit `CMD-R` to build, install, and run the app and follow the instructions.
  </TabItem>

  <TabItem value="macos" label="macOS">
    Ensure the [third-party libraries have been built for macOS](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_macos.md#2-building-the-third-party-libraries). Then follow the instructions to build the Ocean code base. Let's assume you're building Ocean with the release build config:

    ```bash
    cd ${OCEAN_DEVELOPMENT_PATH}

    # Build the third-party libraries
    ./build/cmake/build_thirdparty_linuxunix.sh -c release -l static -b "${HOME}/build_ocean_thirdparty" -i "${HOME}/install_ocean_thirdparty"

    # Build and install Ocean
    ./build/cmake/build_ocean_linuxunix.sh -c release -l static -b "${HOME}/build_ocean" -i "${HOME}/install_ocean" -t "${HOME}/install_ocean_thirdparty"

    # Execute the demo app (a bundle)
    open ${HOME}/install_ocean/macos_static_Release/bin/application_ocean_demo_tracking_similaritytracker_osx.app
    ```

    Alternatively, [generate a XCode project for Ocean](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_macos.md#4-building-the-ocean-ios-demotest-apps) and search for `application_ocean_demo_tracking_similaritytracker_osx` in the schemes at the top. Then hit `CMD-R` to build and run the app.
  </TabItem>

  <TabItem value="win" label="Windows">
    TODO
  </TabItem>

</Tabs>
