/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_SIMILARITY_TRACKER_H
#define META_OCEAN_TRACKING_POINT_SIMILARITY_TRACKER_H

#include "ocean/tracking/point/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements a tracker determining a similarity transformation between two successive video frames.
 * A similarity transformation has four degrees of freedom and contains a rotation, a scale, and a 2D translation (in x- and y- direction) within the image domain.<br>
 * The 3x3 matrix representing the similarity transformation has the following layout:
 * <pre>
 * | a  -b  tx |
 * | b   a  ty |
 * | 0   0   1 |
 * </pre>
 * @ingroup trackingpoint
 */
class OCEAN_TRACKING_POINT_EXPORT SimilarityTracker
{
	public:

		/**
		 * Definition of individual confidence values.
		 */
		enum TrackerConfidence
		{
			/// No similarity could be determined.
			TC_NONE,
			/// The determined similarity may have a bad quality.
			TC_BAD,
			/// The determined similarity has a moderate quality.
			TC_MODERATE,
			/// The determined similarity has a good quality.
			TC_GOOD,
			/// The determined similarity has a very good quality and can be trusted regardless.
			TC_VERY_GOOD
		};

		/**
		 * Definition of individual textureness qualities.
		 */
		enum RegionTextureness
		{
			/// The textureness is unknown.
			RT_UNKNOWN,
			/// The textureness is low (an almost homogeneous region).
			RT_LOW,
			/// The textureness is moderate.
			RT_MODERATE,
			/// The textureness is high (an almost heterogeneous region).
			RT_HIGH
		};

	public:

		/**
		 * Creates a new tracker object.
		 */
		SimilarityTracker() = default;

		/**
		 * Determines the similarity between two successive video frame within a specified sub-region.
		 * @param yFrame The 8bit grayscale frame for which the similarity (in relation to the previous frame) will be determined, with image dimension [40, infinity)x[40, infinity) must be valid
		 * @param previousSubRegion The sub-region (in the previous frame) in which feature points will be determined and tracked to from the previous frame to the current frame, must be valid, must not lie outside the frame
		 * @param currentTprevious Optional resulting 3x3 matrix containing the entire similarity transformation: currentPoint = similarity * previousPoint
		 * @param translation Optional resulting translation between the current and the previous video frame, in pixel, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param rotation Optional resulting rotation between the current and the previous video frame, in radian, with range [-PI, PI]
		 * @param scale Optional resulting scale between the current and the previous video frame, with range (0, infinity)
		 * @param predictedTranslation The predicted translation between the previous frame and the current frame if known, may be based on e.g., a gyro motion
		 * @param trackerConfidence Optional resulting confidence value of the tracker
		 * @param regionTextureness Optional resulting textureness of the provided sub-region
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded, use `trackerConfidence` to check whether the resulting tracking data is valid
		 */
		bool determineSimilarity(const Frame& yFrame, const CV::PixelBoundingBox& previousSubRegion, SquareMatrix3* currentTprevious = nullptr, Vector2* translation = nullptr, Scalar* rotation = nullptr, Scalar* scale = nullptr, const Vector2& predictedTranslation = Vector2(0, 0), TrackerConfidence* trackerConfidence = nullptr, RegionTextureness* regionTextureness = nullptr, Worker* worker = nullptr);

		/**
		 * Returns the frame pyramid of the most recent frame.
		 * @return Frame pyramid of current frame, if any
		 */
		inline const CV::FramePyramid& currentFramePyramid() const;

		/**
		 * Returns the frame pyramid of one of the previous frames (the current key-frame).
		 * @return Frame pyramid of previous frame, if any
		 */
		inline const CV::FramePyramid& keyFramePyramid() const;

		/**
		 * Resets the similarity tracker.
		 */
		inline void reset();

		/**
		 * Calculate an overall confidence values based on a tracker confidence and a region textureness.
		 * @param trackerConfidence The confidence value of the tracker
		 * @param regionTextureness The textureness of the sub-region in which the tracker was applied
		 * @return The resulting overall confidence value, with range [0, 1]
		 */
		static inline float combinedConfidence(const TrackerConfidence trackerConfidence, const RegionTextureness regionTextureness);

	protected:

		/**
		 * Detects feature points in given frame for which a frame pyramid exists.
		 * The points may be determined in a lower image resolution if enough features can be found.
		 * @param yFramePyramid The frame pyramid of the frame in which the feature points will be determined, with pixel format FORMAT_Y8, must be valid with at least one layer
		 * @param subRegion The sub-region inside the frame in which feature points will be determined, specified in the domain of the finest image resolution, must be valid
		 * @param minimalFeaturePoints The minimal number of feature points that must be detected so that the function succeeds, with range [1, infinity)
		 * @param desiredFeaturePoints The desired number of feature points that should be detected in an ideal case, with range [minimalFeaturePoints, infinity)
		 * @param maximalFeaturePoints The maximal number of feature points that will be detected, with range [desiredFeaturePoints, infinity)
		 * @param featurePoints The resulting detected feature points, defined in the domain of the pyramid layer as returned by 'usedLayerIndex'
		 * @param usedLayerIndex The pyramid layer in which the resulting feature points have been detected, with range [0, framePyramid.layers() - 1]
		 * @param regionTextureness The resulting textureness of the sub-region (which is based on the numbers of found feature etc.)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if at least 'minimalFeaturePoints' could be determined
		 */
		static bool determineFeaturePoints(const CV::FramePyramid& yFramePyramid, const CV::PixelBoundingBox& subRegion, const size_t minimalFeaturePoints, const size_t desiredFeaturePoints, const size_t maximalFeaturePoints, Vectors2& featurePoints, unsigned int &usedLayerIndex, RegionTextureness& regionTextureness, Worker* worker = nullptr);

		/**
		 * Determines the similarity transformation between two successive frames.
		 * @param yPreviousFramePyramid The frame pyramid of the previous frame in which the given 'previousPoints' are located, with pixel format FORMAT_Y8, must be valid with at least one layer
		 * @param yCurrentFramePyramid The frame pyramid of the current frame to which the previous points will be tracked, must have the same format and layout as 'yPreviousFramePyramid'
		 * @param previousPoints The previous feature points defined within the domain of the finest pyramid layer of 'yPreviousFramePyramid', at least one
		 * @param roughCurrentPoints Optional the already known rough locations of the previous points with the current frame, one for each previous point, otherwise an empty vector
		 * @param randomGenerator The object to be used to generate random numbers
		 * @param currentTprevious The resulting similarity transformation matching with the resolution of f the finest pyramid layer, transforming points defined in the previous frame to points in the current frame: (currentPoint = currentTprevious * previousPoint)
		 * @param validCorrespondences The resulting indices of all previous feature points that have been used for determination of the similarity transformation
		 * @param coarsestLayerRadius The search radius on the coarsest layer in pixel, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determineSimilarityTransformation(const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, const Vectors2& previousPoints, const Vectors2& roughCurrentPoints, RandomGenerator& randomGenerator, SquareMatrix3& currentTprevious, Indices32& validCorrespondences, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker = nullptr);

	protected:

		/// The frame pyramid of the key frame, one of the previous frames.
		CV::FramePyramid keyFramePyramid_;

		/// The frame pyramid of the current frame.
		CV::FramePyramid currentFramePyramid_;

		/// The random generator object.
		RandomGenerator randomGenerator_;

		/// The similarity since 'previousFramePyramid_' have been updated the last time, defined in the resolution of the usage pyramid layer.
		SquareMatrix3 previous_T_key_ = SquareMatrix3(true);

		/// The image points located in 'previousFramePyramid_', defined in the usage pyramid layer.
		Vectors2 keyFramePoints_;

		/// The frame pyramid layer index in which 'keyFramePoints_' have been determined, -1 if invalid.
		unsigned int keyFramePointsLayerIndex_ = (unsigned int)(-1);
};

inline const CV::FramePyramid& SimilarityTracker::currentFramePyramid() const
{
	return currentFramePyramid_;
}

inline const CV::FramePyramid& SimilarityTracker::keyFramePyramid() const
{
	return keyFramePyramid_;
}

inline void SimilarityTracker::reset()
{
	currentFramePyramid_.clear();
	keyFramePyramid_.clear();

	previous_T_key_.toIdentity();
	keyFramePoints_.clear();
	keyFramePointsLayerIndex_ = (unsigned int)(-1);
}

inline float SimilarityTracker::combinedConfidence(const TrackerConfidence trackerConfidence, const RegionTextureness regionTextureness)
{
	if (trackerConfidence == TC_VERY_GOOD)
	{
		return 1.0f;
	}

	ocean_assert((unsigned int)(trackerConfidence) < 4u);
	ocean_assert((unsigned int)(regionTextureness) < 4u);

	constexpr float confidenceValues[] =
	{
		0.0f, // TC_NONE,      RT_UNKNOWN
		0.1f, // TC_BAD,       RT_LOW
		0.5f, // TC_MODERATE,  RT_MODERATE
		1.0f, // TC_GOOD,      RT_HIGH
	};

	return confidenceValues[(unsigned int)(trackerConfidence)] * confidenceValues[(unsigned int)(regionTextureness)];
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_SIMILARITY_TRACKER_H
