---
title: Geometry
sidebar_position: 1
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Introduction to Ocean's Coordinate Systems

Ocean provides a suite of functions for both Computer Graphics (CG) and Computer Vision (CV), utilizing distinct coordinate systems tailored to each field.
Understanding these coordinate systems is crucial for effectively using Ocean's functionalities.


### Standard Camera Coordinate System

Ocean adopts a straightforward approach to camera coordinate systems, employing a uniform system across all cameras, whether they are physical cameras or virtual views in rendering engines.
By default, Ocean uses a right-handed coordinate system, which is commonly used in Computer Graphics. In this system:

- The **x-axis** points to the right.
- The **y-axis** points upwards.
- The **z-axis** points towards the observer, meaning the camera itself points towards the negative z-space.

This setup is often referred to as the "Computer Graphics" coordinate system for cameras.
Below is an illustration depicting the axes of Ocean's standard camera coordinate system:

<img src={require('@site/static/img/docs/geometry/ocean_coordinatesystem_camera.jpg').default} alt="Image: Ocean's default camera coordinate system used for Computer Graphs and Computer Vision" height="600" className="center-image"/>


### Flipped Camera Coordinate System

In contrast to the CG coordinate system, Computer Vision applications often use a slightly different setup. Commonly in CV:

- The **x-axis** points to the right.
- The **y-axis** points downwards.
- The **z-axis** points away from the observer (the camera points towards the positive z-space)

To accommodate this variation, Ocean introduces the "flipped camera" coordinate system.
Converting from the standard camera to the flipped camera involves a 180-degree rotation around the x-axis. Below is an illustration for clarity:

<img src={require('@site/static/img/docs/geometry/ocean_coordinatesystem_flippedcamera.jpg').default} alt="Image: Ocean's flipped camera coordinate system" height="600" className="center-image"/>


### Consistency Across Functions

In Ocean, every function that refers to a "camera" or uses a variable named "camera" defaults to using the standard CG coordinate system (pointing towards the negative z-space).
This consistency simplifies the integration and implementation processes across rendering, tracking, and mapping pipelines, making Ocean's usage straightforward.


### Descriptive Naming Conventions

Ocean emphasizes clear and descriptive naming for variables related to transformations. For instance:

- `world_T_camera` denotes a 4x4 homogeneous transformation matrix that transforms points from the camera coordinate system to the world coordinate system:
```cpp
worldPoint = world_T_camera * cameraPoint
```

- Conversely, `camera_T_world` represents the inverse transformation, transforming points from the world coordinate system to the camera coordinate system:
```cpp
cameraPoint = camera_T_world * worldPoint
```

- For CV functions using the flipped camera system, the transformation is denoted as `flippedCamera_T_world`, transforming points from the world to the flipped camera coordinate system:
```cpp
flippedCameraPoint = flippedCamera_T_world * worldPoint
```

- Similarly, a homography transforming image points from the left camera to the right camera is labeled as `right_H_left`:

```cpp
rightPoint = right_H_left * leftPoint
```

These naming conventions are designed to enhance clarity and ensure that developers can easily navigate and utilize Ocean's extensive functionalities.
By maintaining consistent and descriptive naming, Ocean helps streamline the development process for both CG and CV applications.


### Flexibility for Developers

Recognizing that some developers may be accustomed to the CV coordinate system, Ocean provides inline or wrapper functions for all popular functionalities.
This allows developers to choose their preferred coordinate system without confusion.
Function and parameter names are uniquely documented and labeled to prevent any mix-ups between functions or coordinate systems.

#### Example of Function Naming Convention

Consider the utility function(s) `Tracking::Utilities::paintPoints*()`.
Two versions of the function exists. The functions are used for visualizing 2D/3D feature correspondences in camera pose tracking, optimization, or verification.

The default function using Ocean's default coordinate system for cameras:

    - [`Tracking::Utilities::paintPoints()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/tracking/Utilities.h#L394)
    - **world_T_camera** - Represents the camera pose with the camera pointing into the negative z-space and y-axis upwards. This transformation converts points defined in the camera's coordinate system into points defined in the world coordinate system.

For developers preferring (or using) the flipped camera pose, Ocean offers a variant of this function:

    - [`Tracking::Utilities::paintPointsIF()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/tracking/Utilities.h#L411)
    - Suffix Explanation: **IF** stands for *Inverted & Flipped*.
    - **flippedCamera_T_world** - Uses an inverted camera pose compared to `paintPoints()` and with the flipped camera pointing into the positive z-space and y-axis downwards. This transformation converts points defined in the world coordinate system into points defined in the coordinate system of the *flipped* camera.

This consistent use of the suffix **IF** to denote variations in coordinate systems or camera orientations is a standard practice across the Ocean codebase.
This method ensures clarity and ease of use, allowing developers to seamlessly integrate with their preferred systems.
