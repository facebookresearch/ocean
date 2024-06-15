---
title: VRNativeApplication
sidebar_position: 3
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

Building on the [NativeApplication](nativeapplication.md), VRNativeApplication introduces a basic VR environment. This demo integrates an OpenXR session and sets up Ocean's OpenGL ES rendering engine, ready to render simple primitive objects.

However, the app does not render any content; it is merely ready to create your own VR experience.

<img src={require('@site/static/img/docs/demoapps/questapps/vrnativeapplication.png').default} alt="Image: Hierarchy of Ocean's Quest environments" width="600" className="center-image"/>

The entire app is realized with only [four source files](https://github.com/facebookresearch/ocean/tree/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplication/quest). The main class, [`OpenXRVRNativeApplication`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplication/quest/OpenXRVRNativeApplication.h#L21), is derived from [`Platform::Meta::Quest::OpenXR::Application::VRNativeApplication`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/ocean/platform/meta/quest/openxr/application/VRNativeApplication.h#L52), which serves as the base class for all of Ocean's Quest applications with integrated OpenXR sessions and OpenGLES-based rendering engines. The base class handles the entire setup of OpenXR (e.g., spaces, actions, compositor layers) and initializes the stereo framebuffer while handling the necessary render calls. Furthermore, controller tracking is enabled and ready to be used.

To transform the demo app into a fully functional VR experience, you simply need to incorporate some 3D content for rendering. This content should be added within the [`onFramebufferInitialized()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplication/quest/OpenXRVRNativeApplication.cpp#L16) event function, which is triggered after the rendering framebuffer has been initialized.

The subsequent demo app, [Renderer](renderer.md), demonstrates precisely this process.


## Features
- **Integrated OpenXR Session**: Unlike the NativeApplication, this demo includes an OpenXR session setup.
- **Integrated Rendering Engine**: Ready to render simple primitive objects using Ocean's OpenGL ES rendering engine.


## Usage
- **Log Verification**: Similar to the NativeApplication, this app does not provide visual feedback upon starting. Use the following commaned to verify its operation:
```
adb logcat -s Ocean
```

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
