---
title: Homography Image Aligner
sidebar_position: 6
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

Image Aligner Demo Application

This demo application features a simple image aligner designed for successive frames, such as those from a movie, live camera, or image sequence. The alignment process relies on a homography, which is calculated by tracking sparse feature points between consecutive frames.

<div class="center-images">
  <img src={require('@site/static/img/docs/demoapps/crossplatformapps/homography_image_aligner_android.jpg').default} alt="Image: The homography image alignment app on Android" height="600" className="center-image"/>
</div>

### Platform-Independent Implementation
The majority of the code is platform-independent and implemented in the HomographyImageAligner class, which is shared across all platforms. However, a few platform-specific files are used for GUI-related purposes. The application generates visual results for each successive image pair from any input medium, creating a seamless alignment experience.

### Output and Visualization

The application produces blended images for each successive frame pair, providing a visual representation of the alignment process. For instance, if the input medium consists of frames `f0`, `f1`, `f2`, `f3`, and so on, the output will include aligned results for `(f0, f1)`, `(f1, f2)`, `(f2, f3)`, and so on. This is visualized by overlaying the frames of each frame pair. The more seamless the overlay appears, the better the image aligner works.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="android" label="Android" default>
    Ensure the [third-party libraries have been built for Android](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_android.md#2-building-the-third-party-libraries) for all of the required Android ABIs. Let's assume the base location for third-party libraries is `${HOME}/install_ocean_thirdparty`, i.e., the Android versions will be located in `${HOME}/install_ocean_thirdparty/android_${ANDROID_ABI}...`.

    ```bash
    # Define this so that Gradle (and CMake) can find the third-party libraries.
    export OCEAN_THIRDPARTY_PATH="${HOME}/install_ocean_thirdparty"

    # Change into the directory with the Gradle config of this project
    cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/tracking/homographyimagealigner/android

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
    Ensure the [third-party libraries have been built for iOS](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#2-building-the-third-party-libraries). Then follow the general [setup for building iOS apps using XCode](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_ios.md#4-building-the-ocean-ios-demotest-apps). Once the generation of the XCode project is complete, open it and search for `application_ocean_demo_tracking_homographyimagealigner_ios` in the scheme at the top. Then hit `CMD-R` to build, install, and run the app and follow the instructions.
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
    open ${HOME}/install_ocean/macos_static_Release/bin/application_ocean_demo_tracking_homographyimagealigner_osx.app
    ```

    Alternatively, [generate a XCode project for Ocean](https://github.com/facebookresearch/ocean/blob/v1.0.0/building_for_macos.md#4-building-the-ocean-ios-demotest-apps) and search for `application_ocean_demo_tracking_homographyimagealigner_osx` in the schemes at the top. Then hit `CMD-R` to build and run the app.
  </TabItem>

  <TabItem value="win" label="Windows">
    TODO
  </TabItem>

</Tabs>
