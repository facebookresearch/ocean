/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_P4P_H
#define META_OCEAN_GEOMETRY_P4P_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/PerspectivePose.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class provides functions determining the camera's pose by a set of four 3D object and 2D image points.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT P4P : public PerspectivePose
{
	public:

		/**
		 * Calculates the pose of a camera by a set of four image and object points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoints Four 3D object points, defined in world, must be valid
		 * @param imagePoints Four 2D image points, one for each 3D object point, must be valid
		 * @param world_T_camera Resulting camera pose, with default camera pointing towards the negative z-space, with y-axis upwards
		 * @return True, if succeeded
		 */
		static bool pose(const AnyCamera& camera, const Vector3* objectPoints, const Vector2* imagePoints, HomogenousMatrix4& world_T_camera);

	protected:

		/**
		 * Calculates four distances between camera center and the four 3D object points.
		 * @param objectPoints Four object points, must be valid
		 * @param imageRays Three 3D rays with unit length, defined in the coordinate system of the camera, starting at the camera's center of projection (equal to the origin), hitting the image plane at image points corresponding to the 3D object points and pointing towards the object points, each ray corresponds to one 3D object point, must be valid
		 * @param distances The four resulting distances between camera center and each of the four object points
		 * @return True, if succeeded
		 */
		static bool calculatePointDistances(const Vector3* objectPoints, const Vector3* imageRays, Scalar distances[4]);
};

}

}

#endif // META_OCEAN_GEOMETRY_P4P_H
