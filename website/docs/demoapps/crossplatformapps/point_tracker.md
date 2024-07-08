---
title: Point Tracker
sidebar_position: 4
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description


This app demonstrates the use of a tracker for sparse 2D feature points.

<div class="center-images">
  <img src={require('@site/static/img/docs/demoapps/crossplatformapps/point_tracker_android.jpg').default} alt="Image: Screenshot of the point tracker app on android" height="400" className="center-image"/>
</div>

### Feature Point Tracking
Feature points are tracked from frame to frame for as long as possible. Additionally, the tracker actively adds new feature points in empty regions, ensuring a steady supply of points to track.

### Database and Visualization
The tracker stores a historical record of all feature points in a database, [`Database`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/tracking/Database.h#L27-L66), efficiently maintaining image coordinates for each point across multiple previous frames. The application visualizes this database by drawing a path for each feature point, showcasing its image coordinates from previous frames – also known as tracks.

### Demo and Implementation
The demo utilizes a platform-independent class called [`PointTrackerWrapper`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/application/ocean/demo/tracking/pointtracker/PointTrackerWrapper.h#L37-L41), which acquires input media and returns an augmented image with point paths (tracks). This class serves as a wrapper around the core tracking logic, implemented in the [`Tracking::Point::PointTracker`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/tracking/point/PointTracker.h#L34-L43) class.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="android" label="Android" default>
    Ensure the [third-party libraries have been built for Android](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#2-building-the-third-party-libraries) for all of the required Android ABIs. Let's assume the base location for third-party libraries is `${HOME}/install_ocean_thirdparty`, i.e., the Android versions will be located in `${HOME}/install_ocean_thirdparty/android_${ANDROID_ABI}...`.

    ```bash
    # Define this so that Gradle (and CMake) can find the third-party libraries.
    export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty"

    # Change into the directory with the Gradle config of this project
    cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/tracking/pointtracker/android

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
    Ensure the [third-party libraries have been built for iOS](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#2-building-the-third-party-libraries). Then follow the general [setup for building iOS apps using XCode](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#4-building-the-ocean-ios-demotest-apps). Once the generation of the XCode project is complete, open it and search for `application_ocean_demo_tracking_pointtracker_ios` in the scheme at the top. Then hit `CMD-R` to build, install, and run the app and follow the instructions.
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
    open ${HOME}/install_ocean/macos_static_Release/bin/application_ocean_tracking_pointtracker_osx.app
    ```

    Alternatively, [generate a XCode project for Ocean](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_macos.md#4-building-the-ocean-ios-demotest-apps) and search for `application_ocean_tracking_pointtracker_osx` in the schemes at the top. Then hit `CMD-R` to build and run the app.
  </TabItem>

  <TabItem value="win" label="Windows">
    TODO
  </TabItem>

</Tabs>
