---
title: Camera Pose
sidebar_position: 2
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

Ocean provides robust tools for estimating camera poses for both mono and stereo cameras, catering to various degrees of freedom (DOF).
Below, we explore the different methods available in Ocean for calculating camera poses, ensuring precision and reliability in computer vision applications.

## Mono Cameras

Ocean offers several functions for estimating camera poses ranging from 3-DOF to 6-DOF.
These functions utilize perspective point problems based on correspondences between 2D image points and 3D object points.

### P3P (Perspective-Three-Point)
* [`P3P::poses()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/P3P.h#L75): This function requires three correspondences between 2D image points and their corresponding 3D object points. It utilizes a camera model, which defines how 3D object points are projected onto the 2D image plane. The camera model is necessary here as it defines the projection of 3D points based on the camera's intrinsic parameters.

```cpp
#include "ocean/math/AnyCamera.h"
#include "ocean/geometry/P3P.h"

AnyCameraPinhole camera(PinholeCamera(1920u, 1080u, Numeric::deg2rad(60)));

Vector3 objectPoints[3] =
{
  Vector3(-1, 0, -1),
  Vector3(0, 0, 1),
  Vector3(1, 0, -1)
};

Vector2 imagePoints[3] =
{
  Vector2(200, 200),
  Vector2(900, 700),
  Vector2(1600, 250)
};

HomogenousMatrix4 world_T_cameras[4];
unsigned int numberPoses = Geometry::P3P::poses(camera, objectPoints, imagePoints, world_T_cameras);

Log::info() << "We have " << numberPoses << " 6-DOF camera poses fitting to the provided 2D/3D correspondences";
```

* [`P3P::poses()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/P3P.h#L91): This variant of the function works with three image rays that originate from the camera's center of projection and extend towards the 3D object points. Since this approach directly uses image rays, it does not require a camera model to project 3D points onto the 2D image plane. This method is beneficial when the camera model is not at hand or when working directly in normalized image coordinates.

```cpp
#include "ocean/geometry/P3P.h"
#include "ocean/math/AnyCamera.h"

AnyCameraPinhole camera(PinholeCamera(1920u, 1080u, Numeric::deg2rad(60)));

Vector3 objectPoints[3] =
{
  Vector3(-1, 0, -1),
  Vector3(0, 0, 1),
  Vector3(1, 0, -1)
};

Vector2 imagePoints[3] =
{
  Vector2(200, 200),
  Vector2(900, 700),
  Vector2(1600, 250)
};

Vector3 imageRays[3] =
{
  camera.vector(imagePoints[0]),
  camera.vector(imagePoints[1]),
  camera.vector(imagePoints[2])
};

HomogenousMatrix4 world_T_cameras[4];
unsigned int numberPoses = Geometry::P3P::poses(objectPoints, imageRays, world_T_cameras);

Log::info() << "We have " << numberPoses << " 6-DOF camera poses fitting to the provided 2D/3D correspondences";
```

P3P can return up to four different camera poses, requiring additional geometrical checks to identify the correct pose.
This method is particularly useful in applications where minimal data points are available for pose estimation.


### P4P (Perspective-Four-Point)
* [`P4P::pose()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/P4P.h#L38): Utilizes four correspondences between 2D image points and their corresponding 3D object points.

The P4P method provides a more robust solution compared to P3P by using an additional point, which helps in reducing ambiguities in the pose estimation process.
However, P4P is significantly slower than P3P and thus may not always be the better choice.


### PnP (Perspective-n-Point)

* [`PnP::pose()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/PnP.h#L39): For scenarios with five or more correspondences between 2D image points and 3D object points.

As long as the point correspondences are free of outliers, PnP is ideal for complex scenes where more correspondences can be established, leading to higher accuracy and stability in the estimated camera pose.


### RANSAC-based P3P

* [`RANSAC::p3p()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/RANSAC.h#L81): This function determines the correct camera pose for point correspondences which contains outliers. Further, it allows for non-linear optimization using the Levenberg-Marquardt algorithm to minimize projection errors.

The RANSAC-based approach is highly effective in real-world scenarios where data may be contaminated with noise, ensuring reliable pose estimation even in challenging conditions.
You will see that `RANSAC::p3p()` is used all over Ocean for all sorts of camera pose functionalities.

```cpp
#include "ocean/geometry/RANSAC.h"
#include "ocean/math/AnyCamera.h"

AnyCameraPinhole camera(PinholeCamera(1920u, 1080u, Numeric::deg2rad(60)));

Vectors3 objectPoints = ...; // 3D object points known from a 3D model
Vectors2 imagePoints = ...; // the known 2D image points, one corresponding 2D image point for each 3D object point
ocean_assert(objectPoints.size() == imagePoints.size());

ConstArrayAccessor<Vector3> objectPointAccessor(objectPoints); // a wrapper to access the object points
ConstArrayAccessor<Vector2> imagePointAccessor(imagePoints);

RandomGenerator randomGenerator;

unsigned int minimalValidCorrespondences = 20; // we expect that at least 50 of our 'objectPoints.size()' correspondences are correct

bool nonLinearOptimization = true; // let's do a non-linear optimization after a valid 6-DOF camera pose has been found

Scalar faultyRate = 0.4; // let's say we expect our data to contain 40% outliers (could also be determined with '1 - minimalValidCorrespondences / objectPoints.size()')
unsigned int ransacIterations = Geometry::RANSAC::iterations(3u, 0.99, faultyRate);

HomogenousMatrix4 world_T_camera;
if (Geometry::RANSAC::p3p(camera, objectPointAccessor, imagePointAccessor, randomGenerator, world_T_camera, minimalValidCorrespondences, nonLinearOptimization, ransacIterations))
{
  Log::info() << "RANSAC was able to determine a valid 6-DOF camera pose for the input data";
}
```

### RANSAC-based Camera orientation

* [`RANSAC::orientation()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/RANSAC.h#L263): Calculates the 3-DOF rotation of the camera in cases where a camera pose can be described entirely based on a rotation, e.g., because the given 3D object points are extremely far away from the camera or the camera pose simply does not contain any translation.


## Stereo Camera Pose Estimation

### Device Pose Calculation

* [`RANSAC::objectTransformationStereo()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/RANSAC.h#L616):
For devices equipped with stereo cameras (e.g., camera rigs or headsets), Ocean can calculate the 6-DOF pose of the device relative to a visible 3D object, provided the calibration of both cameras is known. This method is crucial for applications in virtual and augmented reality, where precise device tracking is essential for immersive experiences.

### Stereo Camera Poses from 2D/2D Correspondences
* [`StereoscopicGeometry::cameraPose()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/StereoscopicGeometry.h#L52):
Calculates the camera poses of two mono cameras using corresponding sets of 2D image points. This method also determines the 3D object points corresponding to the 2D image points in both cameras, essential for stereoscopic geometry.

The 2D/2D correspondences can result from a point tracking approach where 2D features such as corners are tracked across consecutive camera frames. As the camera moves sufficiently (ensuring that the baseline between both cameras is large enough), it becomes possible to calculate both camera poses, albeit up to a scale factor.

In addition to camera pose estimation, Ocean provides robust functions for calculating the essential and fundamental matrices, which are crucial for understanding the epipolar geometry between stereo images. These matrices can be computed with or without the use of RANSAC functionality, offering flexibility depending on the presence of outliers in the data:

* [Essential Matrix](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/EpipolarGeometry.h#L75): Used to recover the relative orientation and position between two cameras. This matrix is particularly useful when the camera intrinsic parameters are known.
* [Fundamental Matrix](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/geometry/RANSAC.h#L369): More general than the essential matrix, as it is used when the camera intrinsics are unknown. It relates corresponding points in stereo images without requiring knowledge of the camera settings.
