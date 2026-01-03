/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_POSE_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_SLAM_POSE_CORRESPONDENCES_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/CameraPose.h"
#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/TrackingCorrespondences.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/GravityConstraints.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class holds 2D-3D point correspondences for camera pose estimation.
 * The class allows memory reuse across frames to minimize allocations.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT PoseCorrespondences
{
	public:

		/**
		 * Creates a new pose correspondences object.
		 */
		PoseCorrespondences();

		/**
		 * Adds a correspondence.
		 * @param objectPoint The 3D object point in world coordinates
		 * @param imagePoint The 2D image point observation
		 * @param objectPointId The unique identifier of the object point
		 * @param precision The localization precision of the object point
		 * @param imagePointSqrDistance The squared distance between previous and current image point (for motion estimation), 0 if unknown
		 */
		inline void addCorrespondence(const Vector3& objectPoint, const Vector2& imagePoint, const Index32 objectPointId, const LocalizedObjectPoint::LocalizationPrecision precision, const Scalar imagePointSqrDistance = 0);

		/**
		 * Resets and prepares pose correspondences from tracking correspondences.
		 * This method clears all previous data and converts valid 2D-2D tracking correspondences into 2D-3D correspondences suitable for pose estimation by extracting object point positions and metadata.
		 * @param trackingCorrespondences The source tracking correspondences with valid flags set
		 */
		void reset(const TrackingCorrespondences& trackingCorrespondences);

		/**
		 * Returns the number of correspondences.
		 * @return The number of correspondences
		 */
		inline size_t size() const;

		/**
		 * Returns whether no correspondences exist.
		 * @return True, if empty
		 */
		inline bool isEmpty() const;

		/**
		 * Estimates the camera pose from the correspondences.
		 * The method first attempts to refine a previous camera pose using non-linear optimization.
		 * If no valid previous pose is available or optimization fails, it falls back to RANSAC-based P3P.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_previousCamera The pose of the previous camera frame, invalid if unavailable
		 * @param minimalNumberCorrespondences The minimal number of inliers required, with range [5, infinity)
		 * @param randomGenerator A random generator for RANSAC
		 * @param maximalProjectionError The maximal projection error for inliers in pixels, with range [0, infinity)
		 * @param estimatorType The robust estimator type (must not be ET_SQUARE)
		 * @param gravityConstraints Optional gravity constraints, nullptr if unused
		 * @param robustError Optional resulting mean squared projection error of inliers, nullptr if not needed
		 * @return The estimated camera pose if successful, nullptr otherwise
		 */
		SharedCameraPose determinePose(const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const unsigned int minimalNumberCorrespondences, RandomGenerator& randomGenerator, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType, const Geometry::GravityConstraints* gravityConstraints, Scalar* robustError = nullptr);

	protected:

		/**
		 * Clears all data for reuse.
		 */
		void clear();

		/**
		 * Filters the arrays to contain only inlier correspondences.
		 * This method uses the indices in inlierIndices_ to filter the arrays, and populates outlierObjectPointIds_ with the IDs of removed correspondences.
		 * After calling this method, only the inlier correspondences remain.
		 */
		void applyInlierSubset();

		/**
		 * Reserves memory for the expected number of correspondences.
		 * @param capacity The expected number of correspondences
		 */
		void reserve(const size_t capacity);

	public:

		/// The 3D object points in world coordinates.
		Vectors3 objectPoints_;

		/// The 2D image point observations, one for each object point.
		Vectors2 imagePoints_;

		/// The unique identifiers of the object points, one for each object point.
		Indices32 objectPointIds_;

		/// The localization precisions of the object points, one for each object point.
		LocalizedObjectPoint::LocalizationPrecisions precisions_;

		/// The squared distances between previous and current image points (for motion estimation), one for each object point.
		Scalars imagePointSqrDistances_;

		/// The IDs of object points that were outliers.
		Indices32 outlierObjectPointIds_;

		/// The IDs of object points that contributed precisely to the pose (for debugging/visualization).
		UnorderedIndexSet32 preciseObjectPointIds_;

		/// The IDs of object points that did not contribute precisely to the pose (for debugging/visualization).
		UnorderedIndexSet32 impreciseObjectPointIds_;

		/// The map version at the time the correspondences were gathered.
		Index32 mapVersion_ = 0u;

	protected:

		/// The indices of inlier correspondences (into the arrays).
		Indices32 inlierIndices_;
};

inline void PoseCorrespondences::addCorrespondence(const Vector3& objectPoint, const Vector2& imagePoint, const Index32 objectPointId, const LocalizedObjectPoint::LocalizationPrecision precision, const Scalar imagePointSqrDistance)
{
	objectPoints_.push_back(objectPoint);
	imagePoints_.push_back(imagePoint);
	objectPointIds_.push_back(objectPointId);
	precisions_.push_back(precision);
	imagePointSqrDistances_.push_back(imagePointSqrDistance);
}

inline size_t PoseCorrespondences::size() const
{
	ocean_assert(objectPoints_.size() == imagePoints_.size());
	ocean_assert(objectPoints_.size() == objectPointIds_.size());

	return objectPoints_.size();
}

inline bool PoseCorrespondences::isEmpty() const
{
	return objectPoints_.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_POSE_CORRESPONDENCES_H
