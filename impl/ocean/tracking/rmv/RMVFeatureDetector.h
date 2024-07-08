/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RMV_FEATURE_DETECTOR_H
#define META_OCEAN_TRACKING_RMV_RMV_FEATURE_DETECTOR_H

#include "ocean/tracking/rmv/RMV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/Box2.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * This class implements an abstraction layer for individual feature detectors.
 * @ingroup trackingrmv
 */
class OCEAN_TRACKING_RMV_EXPORT RMVFeatureDetector
{
	public:

		/**
		 * Definition of individual feature detectors.
		 */
		enum DetectorType
		{
			/// Invalid feature detector.
			DT_INVALID,
			/// FAST feature detector.
			DT_FAST_FEATURE,
			/// Harris corner detector.
			DT_HARRIS_FEATURE,
		};

	public:

		/**
		 * Returns whether the specified detector prefers a smoothed image for tracking.
		 * A smoothed image can improve the feature robustness e.g., for corner detectors.
		 * @param detectorType Type of the detector to check for
		 * @return True, if so
		 */
		static inline bool needSmoothedFrame(const DetectorType detectorType);

		/**
		 * Returns whether the specified detector prefers a pyramid initialization.
		 * @param detectorType Type fo the detector to check for
		 * @return True, if so
		 */
		static inline bool needPyramidInitialization(const DetectorType detectorType);

		/**
		 * Detects features in a given frame and sort them according to their strength.
		 * @param frame The frame to detect features in
		 * @param detectorType Type of the detector to by used
		 * @param threshold Minimal strength threshold all features must exceed
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param worker Optional worker object to distribution the computation
		 * @return Resulting feature positions
		 */
		static Vectors2 detectFeatures(const Frame& frame, const DetectorType detectorType, const Scalar threshold, const bool frameIsUndistorted, Worker* worker = nullptr);

		/**
		 * Detects features in a subregion of a given frame and sort them according to their strength.
		 * @param yFrame Frame to detect features in, must be valid
		 * @param boundingBox Bounding box defining the subregion for feature detection, the area is clamped to the image boundaries, an invalid bounding box to detect feature points in the entire frame
		 * @param detectorType Type of the detector to by used
		 * @param threshold Minimal strength threshold all features must exceed, with range [0, infinity)
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param worker Optional worker object to distribution the computation
		 * @return Resulting feature positions
		 */
		static Vectors2 detectFeatures(const Frame& yFrame, const Box2& boundingBox, const DetectorType detectorType, const Scalar threshold, const bool frameIsUndistorted, Worker* worker = nullptr);

		/**
		 * Detects features in a given frame and sort them according to their strength.
		 * Further, this functions tries to exactly detect a certain number of features.
		 * @param frame The frame to detect features in
		 * @param detectorType Type of the detector to by used
		 * @param approximatedThreshold Approximated minimal strength threshold all features should exceed, however, this threshold will be changed to reach the specified number of feature points
		 * @param numberFeatures Number of feature points to be detected
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param worker Optional worker object to distribution the computation
		 * @return Resulting feature positions
		 */
		static Vectors2 detectFeatures(const Frame& frame, const DetectorType detectorType, const Scalar approximatedThreshold, const size_t numberFeatures, const bool frameIsUndistorted, Worker* worker = nullptr);

		/**
		 * Detects features in a subregion of a given frame and sort them according to their strength.
		 * Further, this functions tries to exactly detect a certain number of features.
		 * @param frame The frame to detect features in
		 * @param boundingBox Bounding box defining the subregion for feature detection, the area is clamped to the image boundaries if extending them
		 * @param detectorType Type of the detector to by used
		 * @param approximatedThreshold Approximated minimal strength threshold all features should exceed, however, this threshold will be changed to reach the specified number of feature points
		 * @param numberFeatures Number of feature points to be detected
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param worker Optional worker object to distribution the computation
		 * @return Resulting feature positions
		 */
		static Vectors2 detectFeatures(const Frame& frame, const Box2& boundingBox, const DetectorType detectorType, const Scalar approximatedThreshold, const size_t numberFeatures, const bool frameIsUndistorted, Worker* worker = nullptr);
};

inline bool RMVFeatureDetector::needSmoothedFrame(const DetectorType detectorType)
{
	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		case DT_HARRIS_FEATURE:
			return true;

		default:
			break;
	};

	ocean_assert(false && "Invalid detector type!");
	return false;
}

inline bool RMVFeatureDetector::needPyramidInitialization(const DetectorType detectorType)
{
	switch (detectorType)
	{
		case DT_FAST_FEATURE:
		case DT_HARRIS_FEATURE:
			return true;

		default:
			break;
	};

	ocean_assert(false && "Invalid detector type!");
	return needSmoothedFrame(detectorType);
}

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RMV_FEATURE_DETECTOR_H
