---
title: Renderer
sidebar_position: 4
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The Renderer app extends the [VRNativeApplication](vrnativeapplication.md) by demonstrating how to render a simple coordinate system into VR. This visual representation helps in understanding spatial orientation in VR environments.

<img src={require('@site/static/img/docs/demoapps/questapps/rendererapplication.png').default} alt="Image: Hierarchy of Ocean's Quest environments" width="600" className="center-image"/>

### Features
- **Coordinate System Visualization**: Displays the Quest's world coordinate system with 1-meter long axes in front of the user. Each axis has an own color (red for x-axis, green for y-axis, blue for z-axis).
- **Custom OpenGL ES Engine Integration**: Demonstrates how a custom OpenGL ES engine could be integrated into the rendering pipeline (disabled by default).


### Usage
- **Immediate Visual Representation**: Unlike the previous apps, this app provides immediate visual feedback by displaying the coordinate system upon app start.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="qust" label="Quest">
    TODO
  </TabItem>

</Tabs>
