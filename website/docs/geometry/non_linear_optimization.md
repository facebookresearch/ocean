---
title: Nonlinear Optimization
sidebar_position: 4
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

Ocean offers a robust suite of tools for optimizing various parameters such as camera poses, 3D object points, transformations like homographies, and even full bundle adjustments.
This page provides an overview of some of the most popular nonlinear optimization functions available in Ocean, all implemented under the Geometry::NonLinearOptimization... classes.

## Overview of Nonlinear Optimizers

Ocean's nonlinear optimizers are implemented using the Levenberg-Marquardt algorithm, which features adjustable and dynamic optimization step sizes.
Additionally, all optimizers support various [robust estimators](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L26) to handle different data conditions:

* [**Square Estimator**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L52): Optimal for data free of outliers and noise.
* [**Linear Estimator**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L66): Suitable for basic applications where a simple linear approach is sufficient.
* [**Huber Estimator**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L84): Ideal for scenarios where the data contains slight outliers or moderate noise.
* [**Tukey Estimator**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L102): A robust estimator capable of automatically identifying and filtering outliers.
* [**Cauchy Estimator**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/Estimator.h#L118): Offers properties similar to the Huber estimator, suitable for mildly noisy data.

## Key Nonlinear Optimization Functions

### Optimize Camera Pose

* [`NonLinearOptimizationPose::optimizePose()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationPose.h#L96): Optimizes a 6-DOF mono camera pose with respect to the projection error between 3D object points and their corresponding 2D image points in the camera.

```cpp
#include "ocean/geometry/NonLinearOptimizationPose.h"

AnyCameraPinhole camera(PinholeCamera(1920u, 1080u, Numeric::deg2rad(60)));

HomogenousMatrix4 world_T_roughCamera = ...; // a rough 6-DOF camera pose which (e.g., from a previous frame or a good guess etc.)

Vectors3 objectPoints = ...; // known 3D object points located in world
Vectors2 imagePoints = ...; // the observations of the object points in the camera, one for each 3D object point

ConstArrayAccessor<Vector3> objectPointAccessor(objectPoints); // a wrapper to access the object points
ConstArrayAccessor<Vector2> imagePointAccessor(imagePoints);

HomogenousMatrix4 world_T_optimizedCamera;
if (Geometry::NonLinearOptimizationPose::optimizePose(camera, world_T_roughCamera, objectPointAccessor, imagePointAccessor, world_T_optimizedCamera))
{
  Log::info() << "The precise 6-DOF camera pose is: " << world_T_optimizedCamera;
}
```

For cases where covariances of point correspondences are known, a [variation](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationPose.h#L123) of this function can optimize the pose considering the known uncertainty.

### Optimize Camera Orientation

* [`NonLinearOptimizationOrientation::optimizeOrientation()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationOrientation.h#L64): Calculates the precise 3-DOF camera orientation based on 2D/3D point correspondences, ideal when the camera pose is solely composed of rotation.

### Optimize Object Points

* [`NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationObjectPoint.h#L151): Determines the precise location of a 3D object point observed in several precisely calibrated cameras.

* [`NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationObjectPoint.h#L506): Conducts a full bundle adjustment to simultaneously refine the locations of 3D object points and the poses of multiple observing cameras.

### Optimize Camera Parameters

* [`NonLinearOptimizationCamera::optimizeCameraObjectPointsPoses()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationCamera.h#L217): Optimizes intrinsic camera parameters (like focal length and distortion parameters) while performing a full bundle adjustment on the dataset.

### Optimize Object Transformation for Stereo Cameras

* [`NonLinearOptimizationTransformation::optimizeObjectTransformationStereo()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationTransformation.h#L105): Precisely determines the rigid transformation for a 3D object observed in several stereo cameras, useful for static objects like keyboards or image patterns.

### Optimize Homography

* [`NonLinearOptimizationHomography::optimizeHomography()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationHomography.h#L105): A straightforward function to compute a robust homography for several 2D image correspondences. Different estimators can be used to adjust the output based on the characteristics of the input data


### Optimize 3D Plane

* [`NonLinearOptimizationPlane::optimizePlane()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/NonLinearOptimizationPlane.h#L79): Optimizes a 3D plane to best fit a given set of 3D object points. This function allows the use of different estimators to fine-tune the results, especially useful when dealing with outliers in the 3D object points.

```cpp
#include "ocean/geometry/NonLinearOptimizationPlane.h"

Plane3 roughPlane = ...; // the already known rough 3D plane

Vectors3 objectPoints = ...; // the 3D object points all lying inside the 3D plane

ConstArrayAccessor<Vector3> objectPointAccessor(objectPoints); // a wrapper to access the object points

unsigned int iterations = 20u;
Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_HUBER; // let's use the robust Huber estimator because out data may contain some noise/outliers

Plane3 optimizedPlane;
if (Geometry::NonLinearOptimizationPlane::optimizePlane(roughPlane, objectPointAccessor, optimizedPlane, iterations, estimator))
{
  Log::info() << "The optimized 3D plane can be described by " << optimizedPlane.pointOnPlane() << " " << optimizedPlane.normal();
}
```
