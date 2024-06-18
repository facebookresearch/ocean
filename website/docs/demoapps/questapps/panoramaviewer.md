---
title: Panorama Viewer
sidebar_position: 8
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The PanoramaViewer demo app demonstrates how a 360-degree image can be utilized to create a teleportation experience in VR. This app introduces the use of visual assets like pictures and offers insights into adjusting properties within the rendering engine.

<img src={require('@site/static/img/docs/demoapps/questapps/panoramaviewer.jpg').default} alt="Image: Screenshot of Panorama Viewer Quest app" width="600" className="center-image"/>

The demo creates a large-radius rendering sphere object in the [`createTexturedSphere()`](https://github.com/facebookresearch/ocean/blob/c6994ae2add1b2fb295ffe7bffa5abdb7bd5e486/impl/application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest/PanoramaViewer.cpp#L100) function. A 360-degree texture is then applied from the inside. By default, the sphere (and texture) would not be visible from the inside due to default geometry visibility settings. This visibility issue is easily rectified by appending a [**PrimitiveAttribute**](https://www.internalfb.com/intern/staticdocs/ocean/doxygen/class_ocean_1_1_rendering_1_1_primitive_attribute.html) to the sphere's attribute set.

Before the texture can be utilized, it must be transferred from the application's asset container to a temporary directory associated with the application. This transfer is facilitated by Ocean's [**ResourceManager**](https://www.internalfb.com/intern/staticdocs/ocean/doxygen/class_ocean_1_1_platform_1_1_android_1_1_resource_manager.html) for Android.

### Features
 - **360-Degree Image Utilization**: Uses a panoramic image to create immersive VR experiences.
 - **Asset Introduction**: Introduces the use of external assets like images in VR applications.


### Application Potential
 - **Tourism and Real Estate**: Ideal for applications in tourism, real estate, and other fields where immersive visualization is beneficial.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
