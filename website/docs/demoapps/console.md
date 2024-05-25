---
title: Console
sidebar_position: 2
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

This demo app is a bare-bone example for how to set up simple console apps with Ocean which can be run on multiple platforms. Here, *console* refers to the lack of any sophisticated UI elements.

The platform-independent code can be found in [`application/ocean/demo/base/console/Console.cpp`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/base/console/Console.cpp#L17-L54). Platform-dependent code is located in sub-directories that match the name of the corresponding platform.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="android" label="Android" default>
    Ensure that the third-party libraries have been built for Android, cf. [build instructions](https://github.com/facebookresearch/ocean/blob/main/building_for_android.md).

    ```bash
    cd ${OCEAN_DEVELOPMENT_PATH}
    cd build/gradle/application/ocean/demo/base/console/android

    ./gradlew assembleDebug -PoceanThirdPartyPath=/tmp/ocean/install/android
    adb install app/build/outputs/apk/debug/app-debug.apk
    ```

    By default, the log output will be displayed on the the screen of the phone as well as in the Android logs, which can be displayed using:

    ```bash
    adb logcat -s Ocean
    ```

    The logging behavior can be changed in [`application/ocean/demo/base/console/android/DemoConsoleActivity.java`](https://github.com/facebookresearch/ocean/blob/main/impl/application/ocean/demo/base/console/android/DemoConsoleActivity.java#L42-L46).

  </TabItem>

  <TabItem value="ios" label="iOS">
    Ensure that the third-party libraries have been built for iOS (static), cf. [build instructions](https://github.com/facebookresearch/ocean/blob/main/building_for_ios.md).

    ```
    cmake -S"${OCEAN_DEVELOPMENT_PATH}" \
        -B"/tmp/ocean/build/ios/static_Debug" \
        -DCMAKE_BUILD_TYPE="Debug" \
        -G Xcode \
        -DCMAKE_TOOLCHAIN_FILE="${OCEAN_DEVELOPMENT_PATH}/build/cmake/ios-cmake/ios.toolchain.cmake" \
        -DPLATFORM="OS64" \
        -DDEPLOYMENT_TARGET="13.0" \
        -DCMAKE_INSTALL_PREFIX="/tmp/ocean/install/ios/static_Debug" \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=XXXXXXXXXX \
        -DBUILD_SHARED_LIBS="OFF"
    ```

    Where `XXXXXXXXXX` needs to be replaced with the value from the field *Organizational Unit* from your Apple Developer certificate. Without it, code signing will fail.

    Then open the generated XCode project that is located inside the build directory:

    ```
    cd /tmp/ocean/build/ios/static_Debug
    open ocean.xcodeproj
    ```

    In XCode, select `application_ocean_demo_base_console_ios` from the list of targets and press `CMD-R` to build, install, and run the app on your device.
  </TabItem>

  <TabItem value="linux" label="Linux">
    This is Linux.
  </TabItem>

  <TabItem value="macos" label="macOS">
    This is macOS.
  </TabItem>

  <TabItem value="win" label="Windows">
    This is Windows.
  </TabItem>
</Tabs>
