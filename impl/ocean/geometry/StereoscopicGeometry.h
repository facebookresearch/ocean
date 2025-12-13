/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_STEREOSCOPIC_GEOMETRY_H
#define META_OCEAN_GEOMETRY_STEREOSCOPIC_GEOMETRY_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/GravityConstraints.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements function for stereoscopic geometry.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT StereoscopicGeometry
{
	public:

		/**
		 * Determines the pose transformation between two given camera frames from which corresponding image point pairs are given.
		 * For each image point in the first frame a corresponding image point in the second frame must be provided.<br>
		 * Further, this function determines the 3D object points which belong to the given image points.<br>
		 * The first camera pose is expected to be the identity camera pose (a default camera in the origin, pointing towards the negative z-space with y-axis upwards.<br>
		 * The function can support outliers in the given point correspondences (to some extend).
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints0 The image points located in the first frame, at least 5
		 * @param imagePoints1 The image points located in the second frame, each point must have a corresponding image point in the first frame with same index
		 * @param randomGenerator Random generator object
		 * @param world_T_camera1 The resulting camera pose for the second camera, with a default camera pose pointing towards the negative z-space with y-axis upwards
		 * @param objectPoints Optional resulting 3D locations of the object points which are visible in both camera frames (the image points are the projections of these object points)
		 * @param validIndices Optional resulting indices of the given point correspondences which are valid regarding the defined error thresholds
		 * @param maxRotationalSqrError The maximal squared pixel error between a projected object point and a corresponding image point so that the pair counts as valid for rotational camera motion
		 * @param maxArbitrarySqrError The maximal squared pixel error between a projected object point and a corresponding image point so that the pair counts as valid for arbitrary camera motion
		 * @param iterations The number of iterations that will be applied finding a better pose result
		 * @param rotationalMotionMinimalValidCorrespondencesPercent The minimal number of valid correspondences (defined as percent of the entire number of correspondences) that are necessary so that the camera motion is accepted to be pure rotational, with range [0, 1]
		 * @param baselineDistance Optional fixed baseline distance between both cameras; if positive, the translation and object points will be scaled so that the distance between both cameras equals this value, with range (0, infinity), -1 to disable
		 * @return True, if succeeded
		 */
		static inline bool cameraPose(const AnyCamera& camera, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera1, Vectors3* objectPoints = nullptr, Indices32* validIndices = nullptr, const Scalar maxRotationalSqrError = Scalar(1.5 * 1.5), const Scalar maxArbitrarySqrError = Scalar(3.5 * 3.5), const unsigned int iterations = 100u, const Scalar rotationalMotionMinimalValidCorrespondencesPercent = Scalar(0.9), const Scalar baselineDistance = Scalar(-1));

		/**
		 * Determines the pose transformation between two given camera frames from which corresponding image point pairs are given.
		 * For each image point in the first frame a corresponding image point in the second frame must be provided.<br>
		 * Further, this function determines the 3D object points which belong to the given image points.<br>
		 * The function can support outliers in the given point correspondences (to some extend).
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints0 The image points located in the first frame, at least 5
		 * @param imagePoints1 The image points located in the second frame, each point must have a corresponding image point in the first frame with same index
		 * @param randomGenerator Random generator object
		 * @param world_T_camera0 The camera pose for the first camera, with a default camera pose pointing towards the negative z-space with y-axis upwards; if valid it will be used as initial pose, may change if gravity constraints are defined; invalid to let the function determine the initial pose for the first camera pose
		 * @param world_T_camera1 The resulting camera pose for the second camera, with a default camera pose pointing towards the negative z-space with y-axis upwards
		 * @param gravityConstraints Optional gravity constraints ensure that the resulting camera poses are aligned with gravity, with two gravity vectors, nullptr otherwise
		 * @param objectPoints Optional resulting 3D locations of the object points which are visible in both camera frames (the image points are the projections of these object points)
		 * @param validIndices Optional resulting indices of the given point correspondences which are valid regarding the defined error thresholds
		 * @param maxRotationalSqrError The maximal squared pixel error between a projected object point and a corresponding image point so that the pair counts as valid for rotational camera motion
		 * @param maxArbitrarySqrError The maximal squared pixel error between a projected object point and a corresponding image point so that the pair counts as valid for arbitrary camera motion
		 * @param iterations The number of iterations that will be applied finding a better pose result
		 * @param rotationalMotionMinimalValidCorrespondencesPercent The minimal number of valid correspondences (defined as percent of the entire number of correspondences) that are necessary so that the camera motion is accepted to be pure rotational, with range [0, 1]
		 * @param baselineDistance Optional fixed baseline distance between both cameras; if positive, the translation and object points will be scaled so that the distance between both cameras equals this value, with range (0, infinity), -1 to disable
		 * @return True, if succeeded
		 */
		static bool cameraPose(const AnyCamera& camera, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera0, HomogenousMatrix4& world_T_camera1, const GravityConstraints* gravityConstraints = nullptr, Vectors3* objectPoints = nullptr, Indices32* validIndices = nullptr, const Scalar maxRotationalSqrError = Scalar(1.5 * 1.5), const Scalar maxArbitrarySqrError = Scalar(3.5 * 3.5), const unsigned int iterations = 100u, const Scalar rotationalMotionMinimalValidCorrespondencesPercent = Scalar(0.9), const Scalar baselineDistance = Scalar(-1));

		/**
		 * Determines valid correspondences between 2D image points and 3D camera points for two individual camera frames concurrently.
		 * Beware: The given camera matrices are not equal to a extrinsic matrix.<br>
		 * Instead, the camera matrices are the extrinsic camera matrix flipped around the x-axis and inverted afterwards.<br>
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera0_T_world The transformation between world and the flipped first camera (a camera pointing towards the positive z-space with y-axis downwards), must be valid
		 * @param flippedCamera1_T_world The transformation between world and the flipped second camera (a camera pointing towards the positive z-space with y-axis downwards), must be valid
		 * @param objectPoints Accessor providing the 3D object points
		 * @param imagePoints0 Accessor providing the 2D image points for the first camera frame, one image point for each 3D object point
		 * @param imagePoints1 Accessor providing the 2D image points for the second camera frame, on image point for each image point in the first frame (and for each 3D object point)
		 * @param validIndices Resulting indices of all valid correspondences
		 * @param maxSqrError The maximal square pixel error between a projected 3D object point and a corresponding 2D image point to count as valid, with range [0, infinity)
		 * @param onlyFrontObjectPoints True, to accept only object points lying in front of both camera frames
		 * @param totalSqrError Optional resulting sum of all square pixel errors for all valid point correspondences (for both frames)
		 * @param minimalValidCorrespondences Optional number of valid correspondences that must be reached, otherwise the function stops without providing a useful result, with range [0, objectPoints.size()]
		 * @return True, if succeeded; False, if the function stops due to the defined minimal number of thresholds, or if no valid correspondence could be found
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints0 The template type of the accessor for the first image points
		 * @tparam TAccessorImagePoints1 The template type of the accessor for the second image points
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints0, typename TAccessorImagePoints1>
		static bool determineValidCorrespondencesIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera0_T_world, const HomogenousMatrix4& flippedCamera1_T_world, const TAccessorObjectPoints& objectPoints, const TAccessorImagePoints0& imagePoints0, const TAccessorImagePoints1& imagePoints1, Indices32& validIndices, const Scalar maxSqrError = Scalar(3.5 * 3.5), const bool onlyFrontObjectPoints = true, Scalar* totalSqrError = nullptr, const size_t minimalValidCorrespondences = 0);
};

inline bool StereoscopicGeometry::cameraPose(const AnyCamera& camera, const ConstIndexedAccessor<Vector2>& accessorImagePoints0, const ConstIndexedAccessor<Vector2>& accessorImagePoints1, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera1, Vectors3* objectPoints, Indices32* validIndices, const Scalar maxRotationalSqrError, const Scalar maxArbitrarySqrError, const unsigned int iterations, const Scalar rotationalMotionMinimalValidCorrespondencesPercent, const Scalar baselineDistance)
{
	HomogenousMatrix4 world_T_camera0(false);

	if (!cameraPose(camera, accessorImagePoints0, accessorImagePoints1, randomGenerator, world_T_camera0, world_T_camera1, nullptr /*gravityConstraints*/, objectPoints, validIndices, maxRotationalSqrError, maxArbitrarySqrError, iterations, rotationalMotionMinimalValidCorrespondencesPercent, baselineDistance))
	{
		return false;
	}

	ocean_assert(world_T_camera0.isIdentity());

	return true;
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints0, typename TAccessorImagePoints1>
bool StereoscopicGeometry::determineValidCorrespondencesIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera0_T_world, const HomogenousMatrix4& flippedCamera1_T_world, const TAccessorObjectPoints& objectPoints, const TAccessorImagePoints0& imagePoints0, const TAccessorImagePoints1& imagePoints1, Indices32& validIndices, const Scalar maxSqrError, const bool onlyFrontObjectPoints, Scalar* totalSqrError, const size_t minimalValidCorrespondences)
{
	ocean_assert(camera.isValid());
	ocean_assert(flippedCamera0_T_world.isValid());
	ocean_assert(flippedCamera1_T_world.isValid());
	ocean_assert(objectPoints.size() == imagePoints0.size() && imagePoints0.size() == imagePoints1.size());
	ocean_assert(maxSqrError >= 0);

	ocean_assert(validIndices.empty());
	validIndices.clear();

	Scalar error = 0;

	for (size_t n = 0u; n < imagePoints0.size(); ++n)
	{
		//  stop if we cannot reach a specified number of valid correspondences anymore
		if (minimalValidCorrespondences != 0 && imagePoints0.size() + validIndices.size() - n < minimalValidCorrespondences)
		{
			return false;
		}

		if (onlyFrontObjectPoints)
		{
			// we do not count this object point if it is located behind at least one camera
			if (AnyCamera::isObjectPointInFrontIF(flippedCamera0_T_world, objectPoints[n]) == false || AnyCamera::isObjectPointInFrontIF(flippedCamera1_T_world, objectPoints[n]) == false)
			{
				continue;
			}
		}

		const Scalar sqrDistance0 = camera.projectToImageIF(flippedCamera0_T_world, objectPoints[n]).sqrDistance(imagePoints0[n]);
		const Scalar sqrDistance1 = camera.projectToImageIF(flippedCamera1_T_world, objectPoints[n]).sqrDistance(imagePoints1[n]);

		if (sqrDistance0 < maxSqrError && sqrDistance1 < maxSqrError)
		{
			validIndices.push_back(Index32(n));

			error += sqrDistance0 + sqrDistance1;
		}
	}

	if (totalSqrError != nullptr)
	{
		*totalSqrError = error;
	}

	return !validIndices.empty();
}

}

}

#endif // META_OCEAN_GEOMETRY_STEREOSCOPIC_GEOMETRY_H
