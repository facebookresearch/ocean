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

| App Name                                                     | Key Features                                                                                   | Use Case                                | Screenshot                                                                                                                                |
|--------------------------------------------------------------|------------------------------------------------------------------------------------------------|-----------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------|
| [NativeApplication](nativeapplication.md)                    | OpenXR-ready, Basic Template                                                                   | Starting point for VR development       |                                                                                                                                           |
| [VRNativeApplication](vrnativeapplication.md)                | Integrated OpenXR Session, OpenGL ES Rendering Engine                                          | Basic VR app development                |                                                                                                                                           |
| [Renderer](renderer.md)                                      | Coordinate System Visualization, Custom OpenGL ES Engine Integration                           | Spatial orientation in VR               |                                                                                                                                           |
| [VRNativeApplicationAdvanced](vrnativeapplicationadvanced.md)| Controller and Hand Tracking, Passthrough, 3D Mesh Loading                                     | Advanced VR app development             |                                                                                                                                           |
| [FingerDistance](fingerdistance.md)                          | Distance Measurement and Visualization, Simple 3D Object Visualization                         | Educational tool for hand interaction   | <img src={require('@site/static/img/docs/demoapps/questapps/fingerdistance.png').default} width="200" className="center-image" />         |
| [HandGestures](handgestures.md)                              | Supported Hand Gestures, Visualization Mode Alteration                                         | Interactive VR experiences              |                                                                                                                                           |
| [PanoramaViewer](panoramaviewer.md)                          | 360-Degree Image Utilization, Asset Introduction                                               | Tourism and Real Estate VR applications |                                                                                                                                           |
| [Microphone](microphone.md)                                  | Microphone Access, Android Permission Handling, Real-Time Audio Processing                     | Audio-based VR interactions             | <img src={require('@site/static/img/docs/demoapps/questapps/microphone.png').default} width="200" className="center-image" />             |
| [Theremin](theremin.md)                                      | Theremin Simulation, Real-Time Sound Generation, Passthrough Activation                        | Musical VR experiences                  |                                                                                                                                           |
| [SolarSystem](soloarsystem.md)                               | Rich VR Experience, Scene Description with X3D, Educational Audio Playback                     | Educational VR experiences              |                                                                                                                                           |
| [Graphs](graphs.md)                                          | Selectable 3D graphs, adjustable rendering modes, Gradient Descent visualization               | 3D graph exploration and algorithms     |                                                                                                                                           |
| [External Camera](externalcamera.md)                         | Supports external USB cameras, customizable settings, real-time display                        | Custom Computer Vision experimentation  |                                                                                                                                           |
