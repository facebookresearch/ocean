---
title: Renderer
sidebar_position: 4
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The Renderer app extends the [VRNativeApplication](vrnativeapplication.md) by demonstrating how to render a simple coordinate system into VR. This visual representation helps in understanding spatial orientation in VR environments.

<img src={require('@site/static/img/docs/demoapps/questapps/rendererapplication.png').default} alt="Image: Hierarchy of Ocean's Quest environments" width="600" className="center-image"/>

Within the [`onFramebufferInitialized()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/renderer/quest/OpenXRRendererApplication.cpp#L26) function, a 3D coordinate system is established. The scene remains static, meaning its appearance does not change over time. Optionally, the app also demonstrates how to integrate a custom render call, for instance, utilizing a third-party rendering engine. This feature provides flexibility for developers looking to customize the rendering process according to specific requirements or to leverage advanced rendering technologies.

<img src={require('@site/static/img/docs/demoapps/questapps/renderer.jpg').default} alt="Image: Screenshot of the Renderer Quest app" width="600" className="center-image"/>

Use this app as a foundational template for developing lightweight VR applications with minimal dependencies. It offers a streamlined approach for those looking to create efficient and straightforward VR experiences.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="qust" label="Quest">
    TODO
  </TabItem>

</Tabs>


## Features
- **Coordinate System Visualization**: Displays the Quest's world coordinate system with 1-meter long axes in front of the user. Each axis has an own color (red for x-axis, green for y-axis, blue for z-axis).
- **Custom OpenGL ES Engine Integration**: Demonstrates how a custom OpenGL ES engine could be integrated into the rendering pipeline (disabled by default).


## Usage
- **Immediate Visual Representation**: Unlike the previous apps, this app provides immediate visual feedback by displaying the coordinate system upon app start.
