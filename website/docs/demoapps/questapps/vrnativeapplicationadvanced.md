---
title: VRNativeApplicationAdvanced
sidebar_position: 5
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The VRNativeApplicationAdvanced is an enhanced version of the [VRNativeApplication](vrnativeapplication.md), incorporating several additional functionalities and features for a more interactive VR experience.

<img src={require('@site/static/img/docs/demoapps/questapps/vrnativeapplicationadvanced.png').default} alt="Image: Hierarchy of Ocean's Quest environments" width="600" className="center-image"/>

Like its predecessors, this demo app is also concise in terms of code. It is derived from [`Platform::Meta::Quest::OpenXR::Application::VRNativeApplicationAdvanced`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h#L52), which, as the name suggests, is designed to facilitate the development of advanced VR (or XR) applications. This app automatically enables the tracking and visualization of both controllers and hands. Additionally, it is configured for passthrough capabilities and supports the loading and visualization of various 3D model file formats, such as glTF, OBJ, and X3D.

The demo app cycles through several visualization modes to render the user's hands. The [`onPreRender()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/vrnativeapplicationadvanced/quest/OpenXRVRNativeApplicationAdvanced.cpp#L16) function is invoked before each new frame is rendered, providing an opportunity to modify the visual content of the XR experience dynamically. This feature allows developers to tailor the visual aspects of the app according to specific user interactions or environmental contexts.

## Features
  - **Controller Tracking and Visualization**: Automatic tracking and visualization of controllers.
 - **Hand Tracking and Rendering**: Supports hand tracking and rendering for more natural interactions.
- **Passthrough Activation**: Allows activation of passthrough for mixed reality applications.
- **3D Mesh Loading**: Supports loading of 3D meshes for visualization and interaction, with formats like glTF, obj, and X3D supported.

- **Coordinate System Visualization**: Displays the Quest's world coordinate system with 1-meter long axes in front of the user. Each axis has an own color (red for x-axis, green for y-axis, blue for z-axis).
- **Custom OpenGL ES Engine Integration**: Demonstrates how a custom OpenGL ES engine could be integrated into the rendering pipeline (disabled by default).


## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
