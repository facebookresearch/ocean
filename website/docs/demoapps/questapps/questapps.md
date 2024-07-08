---
title: Quest Apps
sidebar_position: 1
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Introduction

Welcome to our comprehensive suite of demo applications designed for Meta's [Quest](https://www.meta.com/quest/) headsets, leveraging the powerful [OpenXR](https://www.khronos.org/openxr/) framework. These demos are crafted to showcase a progressive range of functionalities, each building upon the last, to demonstrate the capabilities of VR development on this cutting-edge platform.

The journey through these demo apps is structured to enhance understanding and skills in a logical and incremental manner. Starting with the most fundamental concepts and moving towards more complex implementations, each app serves as a stepping stone to mastering VR development on the Quest.

The first two applications in this series, [**NativeApplication**](nativeapplication.md) and [**VRNativeApplication**](vrnativeapplication.md), serve primarily as template applications. They are foundational but require additional development effort to become fully operational. These initial apps are crucial for developers who are keen on gaining a deep understanding of the basic building blocks of OpenXR and the initial setup required for VR applications on the Quest platform.

However, starting with the third demo app, [**Renderer**](renderer.md), each subsequent application is fully functional on its own. From this point onwards, the demos are designed to not only illustrate specific functionalities but also to provide a hands-on, ready-to-execute experience. Developers interested in diving straight into functional demos, bypassing the foundational OpenXR and rendering topics covered in the first two apps, may choose to start here.

Each app not only adds more complexity but also introduces new features and capabilities, making it a rewarding experience to explore them sequentially. Whether you are a beginner looking to understand the basics or an advanced developer interested in specific functionalities like hand tracking, audio processing, or complex interaction models, there is value in exploring each demo app in the order presented.

We encourage developers to engage with each demo app, starting from where it makes sense based on their interest and expertise level. By progressing through these demos, developers will gain a comprehensive understanding of VR development on the Quest, leveraging the full potential of OpenXR to create immersive and interactive VR experiences.

## App Catalog

| App Name                                                     | Key Features                                                                                   | Use Case                                | Screenshot                                                                                                                                       |
|--------------------------------------------------------------|------------------------------------------------------------------------------------------------|-----------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------|
| [NativeApplication](nativeapplication.md)                    | OpenXR-ready, Basic Template                                                                   | Starting point for VR development       | <img src={require('@site/static/img/docs/demoapps/questapps/nativeapplication.png').default} width="120" className="center-image"/>              |
| [VRNativeApplication](vrnativeapplication.md)                | Integrated OpenXR Session, OpenGL ES Rendering Engine                                          | Basic VR app development                | <img src={require('@site/static/img/docs/demoapps/questapps/vrnativeapplication.png').default} width="120" className="center-image"/>            |
| [Renderer](renderer.md)                                      | Coordinate System Visualization, Custom OpenGL ES Engine Integration                           | Spatial orientation in VR               | <img src={require('@site/static/img/docs/demoapps/questapps/renderer.jpg').default} width="200" className="center-image" />                      |
| [VRNativeApplicationAdvanced](vrnativeapplicationadvanced.md)| Controller and Hand Tracking, Passthrough, 3D Mesh Loading                                     | Advanced VR app development             | <img src={require('@site/static/img/docs/demoapps/questapps/vrnativeapplicationadvanced.jpg').default} width="200" className="center-image" />   |
| [Finger Distance](fingerdistance.md)                         | Distance Measurement and Visualization, Simple 3D Object Visualization                         | Educational tool for hand interaction   | <img src={require('@site/static/img/docs/demoapps/questapps/fingerdistance.png').default} width="200" className="center-image" />                |
| [Hand Gestures](handgestures.md)                             | Supported Hand Gestures, Visualization Mode Alteration                                         | Interactive VR experiences              | <img src={require('@site/static/img/docs/demoapps/questapps/handgestures.jpg').default} width="200" className="center-image" />                  |
| [Panorama Viewer](panoramaviewer.md)                         | 360-Degree Image Utilization, Asset Introduction                                               | Tourism and Real Estate VR applications | <img src={require('@site/static/img/docs/demoapps/questapps/panoramaviewer.jpg').default} width="200" className="center-image" />                |
| [Microphone](microphone.md)                                  | Microphone Access, Android Permission Handling, Real-Time Audio Processing                     | Audio-based VR interactions             | <img src={require('@site/static/img/docs/demoapps/questapps/microphone.png').default} width="200" className="center-image" />                    |
| [Theremin](theremin.md)                                      | Theremin Simulation, Real-Time Sound Generation, Passthrough Activation                        | Musical VR experiences                  | <img src={require('@site/static/img/docs/demoapps/questapps/theremin.jpg').default} width="200" className="center-image" />                      |
| [Solar System](solarsystem.md)                               | Rich VR Experience, Scene Description with X3D, Educational Audio Playback                     | Educational VR experiences              | <img src={require('@site/static/img/docs/demoapps/questapps/solarsystem.jpg').default} width="200" className="center-image" />                   |
| [Graphs](graphs.md)                                          | Selectable 3D graphs, adjustable rendering modes, Gradient Descent visualization               | 3D graph exploration and algorithms     | <img src={require('@site/static/img/docs/demoapps/questapps/graphs.jpg').default} width="200" className="center-image" />                        |
| [External Camera](externalcamera.md)                         | Supports external USB cameras, customizable settings, real-time display                        | Custom Computer Vision experimentation  | <img src={require('@site/static/img/docs/demoapps/questapps/externalcamera.jpg').default} width="200" className="center-image" />                |

## Debugging Native Quest Apps

Debugging native apps on Android platforms, including Quest, can present some challenges. To facilitate this process, you can utilize the Ocean log mechanism for basic debugging and verification tasks.
To access the logs from a connected Quest device, use logcat by entering the following command for unfiltered logs:

```bash
adb logcat
```

If the log output is overwhelming, you can filter it to show only Ocean-specific logs to make it more manageable by using:

```bash
adb logcat -s Ocean
```

Note that Ocean's debug logs are only available when you run an Ocean app in debug mode. Please refer to Ocean's build instructions to learn how to build debug or release binaries.

For debugging crashes that might involve issues between the Java and native components, it's useful to examine the broader Android runtime logs:

```bash
adb logcat -s AndroidRuntime
```
This approach can provide valuable insights into the underlying issues. Happy debugging!
