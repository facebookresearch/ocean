---
title: NativeApplication
sidebar_position: 2
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The NativeApplication demo is the most basic example of a native application that can be executed on OpenXR-compatible devices like Meta's Quest headsets. It is designed to be OpenXR-ready, providing a foundational template for developers to build upon. However, the OpenXR session needs to be implemented before it can be utilized.

Use this demo app as a starting point if you already have your own OpenXR framework which you intend to use, if you want to implement your own OpenXR codebase, or if you need to debug extreme low-level issues.

The entire app is realized with only [four source files](https://github.com/facebookresearch/ocean/tree/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest). The main class, [`OpenXRNativeApplication`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest/OpenXRNativeApplication.h#L24), is derived from [`Platform::Meta::Quest::OpenXR::Application::NativeApplication`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/ocean/platform/meta/quest/openxr/application/NativeApplication.h#L49), which is the base class for all of Ocean's Quest applications. This base class primarily handles Android-specific application logic, such as the main loop, Android Activity event calls, or Android permission handling.

The demo app requires the implementation of three different event functions:
- [`createOpenXRSession()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest/OpenXRNativeApplication.cpp#L16): This event function is called at the application's start. When invoked, the OpenXR session needs to be initialized.
- [`releaseOpenXRSession()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest/OpenXRNativeApplication.cpp#L23): This event function is called when the application is closed. Use this function to release all OpenXR-related resources.
- [`onIdle()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest/OpenXRNativeApplication.cpp#L28): This is the application's main loop function, which will be called whenever the app is idling. Use this function to handle or execute rendering calls and interactions.

Clearly, this demo app is extremely low-level and requires significant additional effort before achieving a fully functional XR experience. Please refer to Ocean's more advanced Quest demo apps to learn how to start with a completely functional XR environment, ready for rendering and interacting with virtual content.

## Features
- **OpenXR-Ready**: The application is prepared for OpenXR integration, although the session itself needs to be manually added by the developer.
- **Basic Template**: Serves as a starting point for more complex applications.

## Usage
- **Log Verification**: No visual feedback is provided when launching the app on Quest. Developers can verify functionality by accessing the Quest's log using the command:
```
adb logcat -s Ocean
```

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
