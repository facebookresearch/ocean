# Building for Android

This document describes the process to build Ocean for Android. It covers:

1. General requirements
2. Building required third-party libraries
3. Using Ocean in external projects
4. Building Android apps that come with Ocean

## 1 Prerequisites

* [General prerequisites listed on the main page](README.md)
* Install an Android SDK and NDK (Android API 34), for example using [Android Studio](https://developer.android.com/studio). Other versions may work as well but haven't been tested.
* Install a Java Development Kit (JDK), for example [OpenJDK 22](https://jdk.java.net/22/).
* To build the Ocean Android demo/test apps, install [Gradle 8.7](https://www.gradle.org)
* Define the following environment variables:
  * `ANDROID_HOME` - points to the location of the Android SDK, for example: `${HOME}/Library/Android/sdk` (on macOS)
  * `ANDROID_NDK` - points to the location of the Android NDK, for example: `${HOME}/Library/Android/sdk/ndk/26.2.11394342` (on macOS)
  * `ANDROID_NDK_VERSION` - Android NDK version number, for example: `26.2.11394342`
    * May be part of `ANDROID_NDK` path or found as "Pkg.BaseRevision" property listed in `${ANDROID_NDK}/source.properties`
  * `JAVA_HOME` - points to the location of the JDK, for example: `/Library/Java/JavaVirtualMachines/openjdk-22.0.1.jdk/Contents/Home` (on macOS)

## 2 Building the third-party libraries

As with the desktop use case, this process consists of two steps:

1. Building the required third-party libraries
2. Building the Ocean libraries

These steps need to be repeated for all Android ABIs required.

The easiest way to build the third-party libraries is by using the provided build scripts, [`build/cmake/build_thirdparty_android.sh`](build/cmake/build_thirdparty_android.sh). Simply comment out all build configurations that are not required for your project, for example:

```
# run_build_for_android armeabi-v7a android-32 Debug static
# run_build_for_android arm64-v8a   android-32 Debug static
# run_build_for_android x86         android-32 Debug static
# run_build_for_android x86_64      android-32 Debug static

run_build_for_android armeabi-v7a android-32 Debug shared
run_build_for_android arm64-v8a   android-32 Debug shared
# run_build_for_android x86         android-32 Debug shared
# run_build_for_android x86_64      android-32 Debug shared

# run_build_for_android armeabi-v7a android-32 Release static
# run_build_for_android arm64-v8a   android-32 Release static
# run_build_for_android x86         android-32 Release static
# run_build_for_android x86_64      android-32 Release static

run_build_for_android armeabi-v7a android-32 Release shared
run_build_for_android arm64-v8a   android-32 Release shared
# run_build_for_android x86         android-32 Release shared
# run_build_for_android x86_64      android-32 Release shared
```

Once the scripts completes, all binaries and include files of the third-party libraries will have been installed into `/tmp/ocean/install/android/${ANDROID_ABI}_${LINKING_TYPE_${BUILD_TYPE}`.

## 3 Using Ocean in external Android projects

This section provides an example of how to build the Ocean libraries so that they can be integrated into an existing Android project.

First, build the required third-party libraries as described above for the required Android ABIs. Then take a look at the build script for Android-builds of Ocean, [`build/cmake/build_ocean_android.sh`](build/cmake/build_ocean_android.sh), and comment out all build configurations that are not required. Make sure the selection of enabled build configurations matches the one from the build of the third-party libraries.

All binaries and include files of Ocean will have been installed into `/tmp/ocean/install/android/${ANDROID_ABI}_${LINKING_TYPE_${BUILD_TYPE}`.

At this point, Ocean can be integrated into any external project. Projects that use Gradle as their main build system can take advantage of `externalNativeBuild` to build Ocean directly by using adding something similar to the following to their configuration:

```
externalNativeBuild {
  cmake {
    arguments += "-DBUILD_SHARED_LIBS=ON"
    arguments +=
        "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
    targets += "application_ocean_demo_base_console_android_native"
  }
}
```

For a full example, please take a look at the Gradle configuration of the Ocean Android apps, for example [`build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts`](build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts).

For projects using build systems of than Gradle, the precise details of the integration of Ocean are beyond the scope of this document and are left to the reader.

## 4 Building the Ocean Android demo/test apps

First, build the required third-party libraries as described above for the required Android ABIs. Then find the Gradle configuration of Ocean Android app that you want to build, for example [`build/gradle/application/ocean/demo/base/console/android`](build/gradle/application/ocean/demo/base/console/android/app/build.gradle.kts):

```
cd ${OCEAN_DEVELOPMENT_PATH}/build/gradle/application/ocean/demo/base/console/android

# Build the APK of the application
./gradlew build -PoceanThirdPartyPath=/tmp/ocean/install/android

# Install the app
adb install app/build/outputs/apk/debug/app-debug.apk
```

### Known issue: debug vs. release in Gradle

Currently, code signing is not configured in any of the Gradle configs. As a result, Gradle seems to only build debug versions of the apps and also uses debug version of Ocean and its third-party libraries. This may have performance implications. Fixing this is on our TODO list.

Until then, a work-around to use the release builds of the native Ocean code in the Android apps is to force it to always link the release builds. This can be achieved by changing the configuration

1. in the main `CMakeLists.txt` as follows:

```
if (ANDROID)
    # If specified, use the following variable to determine the location of the third-party
    # libraries that are required. This is provided by Gradle.
    if (OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE)
#        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
#            set(CMAKE_FIND_ROOT_PATH ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}/${ANDROID_ABI}_shared_Debug)
#        else()
            set(CMAKE_FIND_ROOT_PATH ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}/${ANDROID_ABI}_shared_Release)
#        endif()

        message(STATUS "OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE = ${OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE}")
    else()
        message(STATUS "OCEAN_THIRD_PARTY_ROOT_FROM_GRADLE = [not defined]")
    endif()
endif()
```

2. and by adding `arguments += "-DCMAKE_BUILD_TYPE=Release"` to the Gradle configuration in `app/build.gradle.kts` to:

```
    externalNativeBuild {
      cmake {
        arguments += "-DBUILD_SHARED_LIBS=ON"
        arguments += "-DCMAKE_BUILD_TYPE=Release"
        arguments +=
            "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
        targets += "application_ocean_demo_base_console_android_native"
      }
```
