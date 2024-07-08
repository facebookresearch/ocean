/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_PNP_H
#define META_OCEAN_GEOMETRY_PNP_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/PerspectivePose.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class provides functions determining the camera's pose based on at least five 3D object and corresponding 2D image points.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT PnP : public PerspectivePose
{
	public:

		/**
		 * Calculates the pose of a camera by a set of at least five image and object points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoints The 3D object points for which the camera pose will be determined, defined in world, must be valid
		 * @param imagePoints The 2D image points located in the camera image, one for each 3D object point, must be valid
		 * @param correspondences Number of point correspondences, with range [5, infinity)
		 * @param world_T_camera Resulting camera pose, with default camera pointing towards the negative z-space, with y-axis upwards
		 * @return True, if succeeded
		 */
		static bool pose(const AnyCamera& camera, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, HomogenousMatrix4& world_T_camera);

	protected:

		/**
		 * Calculates distances between camera center and at least five 3D object points.
		 * @param objectPoints The object points for which the distances will be determined, must be valid
		 * @param imageRays Three 3D rays with unit length, defined in the coordinate system of the camera, starting at the camera's center of projection (equal to the origin), hitting the image plane at image points corresponding to the 3D object points and pointing towards the object points, each ray corresponds to one 3D object point, must be valid
		 * @param correspondences Number of point correspondences, with range [5, infinity)
		 * @param distances Resulting distances between camera center and each of the object points
		 * @return True, if succeeded
		 */
		static bool calculatePointDistances(const Vector3* objectPoints, const Vector3* imageRays, const size_t correspondences, Scalars &distances);

		/**
		 * Calculates the eighth degree polynomial resulting from three law of cosine polynomials.
		 * It uses the method of Sylvester resultants.<br>
		 * <pre>
		 * g(x) = a4 x^8 + a3 x^6 + a2 x^4 + a1 x1^2 + a0 = 0
		 * </pre>
		 * @param cos12 Cosine value of angle between first and second leg
		 * @param cos13 Cosine value of angle between first and third leg
		 * @param cos23 Cosine value of angle between second and third leg
		 * @param d12_2 Square of distance between first and second object point
		 * @param d13_2 Square of distance between first and third object point
		 * @param d23_2 Square of distance between second and third object point
		 * @param a0 Resulting parameter for x1^0
		 * @param a1 Resulting parameter for x1^2
		 * @param a2 Resulting parameter for x1^4
		 * @param a3 Resulting parameter for x1^6
		 * @param a4 Resulting parameter for x1^8
		 * @return True, if succeeded
		 */
		static bool calculateResultant(const Scalar cos12, const Scalar cos13, const Scalar cos23, const Scalar d12_2, const Scalar d13_2, const Scalar d23_2, Scalar& a0, Scalar& a1, Scalar& a2, Scalar& a3, Scalar& a4);

		/**
		 * Calculates the eighth degree polynomial resulting from three law of cosine polynomials.
		 * It uses the method of Sylvester resultants.<br>
		 * It's optimized to performance.
		 * <pre>
		 * g(x) = a4 x^8 + a3 x^6 + a2 x^4 + a1 x1^2 + a0 = 0
		 * </pre>
		 * @param cos12 Cosine value of angle between first and second leg
		 * @param cos13 Cosine value of angle between first and third leg
		 * @param cos23 Cosine value of angle between second and third leg
		 * @param d12_2 Square of distance between first and second object point
		 * @param d13_2 Square of distance between first and third object point
		 * @param d23_2 Square of distance between second and third object point
		 * @param a0 Resulting parameter for x1^0
		 * @param a1 Resulting parameter for x1^2
		 * @param a2 Resulting parameter for x1^4
		 * @param a3 Resulting parameter for x1^6
		 * @param a4 Resulting parameter for x1^8
		 * @return True, if succeeded
		 */
		static bool calculateResultantOptimized(const Scalar cos12, const Scalar cos13, const Scalar cos23, const Scalar d12_2, const Scalar d13_2, const Scalar d23_2, Scalar& a0, Scalar& a1, Scalar& a2, Scalar& a3, Scalar& a4);
};

}

}

#endif // META_OCEAN_GEOMETRY_PNP_H
