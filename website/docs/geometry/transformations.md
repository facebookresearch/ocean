---
title: Transformations
sidebar_position: 3
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Introduction

Ocean offers a comprehensive suite of tools for calculating various transformations between two images or sets of corresponding 2D image points.
These transformations are crucial for tasks such as image alignment, 3D reconstruction, and motion tracking.

## Homography Transformation
Homography is an 8-DOF transformation that includes translation, rotation, scale, aspect ratio, shear, and perspective changes between two images.
Ocean provides several functions to calculate the homography based on different types of input data:

    - [**Known Camera Rotation and Profile**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L46): If the camera rotation and the camera profile are known, this function can be used to compute the homography.

    - [**Known 6-DOF Camera Poses and 3D Plane**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L97): This function calculates the homography for a known 3D plane based on two 6-DOF camera poses.

    - [**Known 2D Image Point Correspondences**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L132): For 2D image point correspondences known in two individual images, Ocean offers SVD-based or linear estimation methods to compute the homography.

    - [**Known 2D Image and Line Correspondences**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L162): If both 2D image point and line correspondences are known, this function provides a specialized approach to calculate the homography.

## Affine Transformation
[Affine transformation](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L218) is a 6-DOF transformation that includes translation, rotation, scale, aspect ratio, and shear. It is used to transform corresponding 2D image points in two individual images.

## Similarity Transformation
This 4-DOF [transformation](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L238) calculates translation, rotation, and scale between corresponding 2D image points in two individual images. It is simpler than affine transformation and is often used when the shape of objects must be preserved.

## Homothetic Transformation
[Homothetic transformation](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L257) is a 3-DOF transformation that calculates translation and scale between corresponding 2D image points in two individual images. This transformation is useful when only size and position changes occur between images.

## Absolute Transformation
Absolute transformation is a crucial concept in computer vision, particularly in scenarios involving 3D data. Ocean provides robust functions for calculating 6-DOF transformations between corresponding 3D object points and camera poses, facilitating accurate alignment and comparison between different datasets or viewpoints.

- [**Transformation Between 3D Object Points**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/AbsoluteTransformation.h#L56): This 6-DOF transformation adjusts one set of 3D object points to align with another set, effectively transforming the points from one coordinate system to another. This is particularly useful in 3D reconstruction and object tracking where consistency across different views is required.

- [**Transformation Between 6-DOF Camera Poses**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/AbsoluteTransformation.h#L70): Similarly, this function computes a 6-DOF transformation that aligns one set of camera poses with another. This capability is essential for applications like multi-view stereo and motion analysis, where the relative positions and orientations of cameras need to be precisely understood and adjusted.

Absolute transformations are integral for ensuring that data collected from different sensors or at different times can be accurately integrated and analyzed within a unified framework.

## Additional Resources
In addition to the above transformations, Ocean includes several helper functions that can simplify the conversion, factorization, or usage of transformations significantly.
For a deeper understanding and more detailed examples, explore the [`Geometry::Homography`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Homography.h#L32), [`Geometry::AbsoluteTransformation`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/AbsoluteTransformation.h#L26) and [`Geometry::Normalization`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Normalization.h#L25) sections in the Ocean framework.
