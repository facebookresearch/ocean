/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RANDOMIZED_POSE_H
#define META_OCEAN_TRACKING_RMV_RANDOMIZED_POSE_H

#include "ocean/tracking/rmv/RMV.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Box3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * This class implements functions for randomized poses.
 * @ingroup trackingrmv
 */
class OCEAN_TRACKING_RMV_EXPORT RandomizedPose
{
	public:

		/**
		 * Determines several random camera poses close to a hemisphere, separated at individual location but slightly randomized.
		 * First, 'rollSteps' randomized poses are scattered around the north pole.<br>
		 * Second, a ring of poses around the hemisphere will be created at 'longitudeSteps' individual locations with mainly 'rollSteps' individual orientations (but all slightly randomized).
		 * @param box The bounding box of the object to be observed, must be valid
		 * @param randomGenerator Random number generator to be used
		 * @param distance The radius of the sphere around the center of the bounding box
		 * @param longitudeSteps The number of longitude steps, with range [1, infinity)
		 * @param rollSteps The number of roll steps, with range [1, infinity)
		 * @return The resulting camera poses slightly randomized but scattered around the hemisphere
		 */
		static HomogenousMatrices4 hemispherePoses(const Box3& box, RandomGenerator& randomGenerator, const Scalar distance, const unsigned int longitudeSteps = 12u, const unsigned int rollSteps = 12u);

		/**
		 * Returns a randomized pose looking at a box.
		 * The pose points to the box center (in a hemisphere with positive y-axis as up vector).
		 * @param box Box to be looked at
		 * @param distance Pose distance to the box center
		 * @param minYFactor Minimal y translation value of the pose in relation to the distance, with range [0, 1)
		 * @return Resulting randomized pose
		 */
		static HomogenousMatrix4 constantDistance(const Box3& box, const Scalar distance, const Scalar minYFactor);

		/**
		 * Returns a set of random pose looking at a box.
		 * The poses points to the box center (in a hemisphere with positive y-axis as up vector).
		 * @param box Box to be locked at
		 * @param distance Pose distance to the box center
		 * @param minYFactor Minimal y translation value of the pose in relation to the distance, with range [0, 1)
		 * @param number Number of random poses to return
		 * @param maximalIterations Number of maximal iterations with no new found pose, with range [10, infinity)
		 * @param angleOffset Minimal angle offset between each now pose, in radian with range (0, PI)
		 * @param poses Resulting poses, must provide enough space for all requested poses
		 */
		static void constantDistance(const Box3& box, const Scalar distance, const Scalar minYFactor, const unsigned int number, const unsigned int maximalIterations, const Scalar angleOffset, HomogenousMatrix4* poses);

		/**
		 * Returns a randomized pose looking at a box.
		 * The pose points to the box with positive y-axis as up vector.
		 * @param pinholeCamera The pinhole camera object to be used for tracking, must be valid
		 * @param box Box to be looked at, must be valid
		 * @param randomGenerator Random generator to be used
		 * @param minDistance Minimal distance to the box's center, with range (0, infinity)
		 * @param maxDistance Maximal distance to the box's center, with range (minDistance, infinity)
		 * @param visibleRatio Defines the minimal ratio between projected box and image size (1 - visibleRatio), and the maximal ratio between projected box and image size (1 + visibleRatio), with range [0, 1)
		 * @return Resulting randomized pose
		 */
		static HomogenousMatrix4 randomPose(const PinholeCamera& pinholeCamera, const Box3& box, RandomGenerator& randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio = Scalar(0.25));

		/**
		 * Returns a set of randomized pose looking at a box.
		 * The pose points to the box with positive y-axis as up vector.
		 * @param pinholeCamera The pinhole camera object defining the projection, must be valid
		 * @param box Box to be looked at, must be valid
		 * @param randomGenerator Random generator to be used
		 * @param minDistance Minimal distance to the box's center, with range (0, infinity)
		 * @param maxDistance Maximal distance to the box's center, with range (minDistance, infinity)
		 * @param visibleRatio Defines the minimal ratio between projected box and image size (1 - visibleRatio), and the maximal ratio between projected box and image size (1 + visibleRatio), with range [0, 1)
		 * @param number Number of random poses to return, with range [1, infinity)
		 * @param poses Resulting poses, must provide enough space for all requested poses
		 * @param worker Optional worker object to distribute the computation
		 */
		static void randomPoses(const PinholeCamera& pinholeCamera, const Box3& box, RandomGenerator& randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio, const size_t number, HomogenousMatrix4* poses, Worker* worker = nullptr);

	private:

		/**
		 * Returns a subset of randomized pose looking at a box.
		 * The pose points to the box with positive y-axis as up vector.
		 * @param pinholeCamera The pinhole camera object defining the projection, must be valid
		 * @param box Box to be looked at, must be valid
		 * @param randomGenerator Random generator to be used
		 * @param minDistance Minimal distance to the box's center, with range (0, infinity)
		 * @param maxDistance Maximal distance to the box's center, with range (minDistance, infinity)
		 * @param visibleRatio Defines the minimal ratio between projected box and image size (1 - visibleRatio), and the maximal ratio between projected box and image size (1 + visibleRatio), with range [0, 1)
		 * @param poses Resulting poses, must provide enough space for all requested poses
		 * @param firstPose First pose to be handled, with range [0, numberPoses)
		 * @param numberPoses Number of poses to be generated, with range [1, numberPoses]
		 */
		static void randomPoseSubset(const PinholeCamera* pinholeCamera, const Box3* box, RandomGenerator* randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio, HomogenousMatrix4* poses, const unsigned int firstPose, const unsigned int numberPoses);
};

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RANDOMIZED_POSE_H
