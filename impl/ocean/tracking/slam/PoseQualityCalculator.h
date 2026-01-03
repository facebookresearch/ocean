/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_POSE_QUALITY_CALCULATOR_H
#define META_OCEAN_TRACKING_SLAM_POSE_QUALITY_CALCULATOR_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/CameraPose.h"
#include "ocean/tracking/slam/LocalizedObjectPoint.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * Helper class allowing to determine the quality of the camera pose based on the number of object points used to determine the camera pose.
 */
class OCEAN_TRACKING_SLAM_EXPORT PoseQualityCalculator
{
	public:

		/**
		 * Creates a new quality calculator object.
		 */
		PoseQualityCalculator() = default;

		/**
		 * Adds an object point with the specified localization precision.
		 * @param localizationPrecision The localization precision of the object point, must not be LP_INVALID
		 */
		void addObjectPoint(const LocalizedObjectPoint::LocalizationPrecision localizationPrecision);

		/**
		 * Returns the number of object points which have been used to determine the camera pose.
		 * @return The pose's object points, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Returns the quality of the camera pose based on the number and precision of object points.
		 * @return The pose quality
		 */
		CameraPose::PoseQuality poseQuality() const;

		/**
		 * Returns a string representation of the pose quality and object point statistics.
		 * @return The string representation
		 */
		std::string toString() const;

	public:

		/// The number of high precision object points used to determine the camera pose, with range [0, infinity).
		size_t numberHighPrecision_ = 0;

		/// The number of medium precision object points used to determine the camera pose, with range [0, infinity).
		size_t numberMediumPrecision_ = 0;

		/// The number of low precision object points used to determine the camera pose, with range [0, infinity).
		size_t numberLowPrecision_ = 0;

		/// The number of object points with unknown precision used to determine the camera pose, with range [0, infinity).
		size_t numberUnknownPrecision_ = 0;
};

inline size_t PoseQualityCalculator::size() const
{
	return numberHighPrecision_ + numberMediumPrecision_ + numberLowPrecision_ + numberUnknownPrecision_;
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_POSE_QUALITY_CALCULATOR_H
