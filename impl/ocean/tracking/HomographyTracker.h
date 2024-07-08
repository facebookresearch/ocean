/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_HOMOGRAPHY_TRACKER_H
#define META_OCEAN_TRACKING_HOMOGRAPHY_TRACKER_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a homography tracker able to determine a homography in real-time.
 * The tracker stores the two frame pyramids, one pyramid for the previous frame, one pyramid of the current frame.<br>
 * For each new camera frame a group of reliable/strong feature points visible in the previous camera frame will be tracked to the new camera frame.<br>
 * The point correspondences will be used to calculate the homography.<br>
 * In contrast to HomographyImageAlignmentSparse the HomographyTracker uses explicitly provided image points for the determination of the homography while the HomographyImageAlignmentSparse selects suitable sparse information on its own.
 * @see HomographyImageAlignmentSparse.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT HomographyTracker
{
	public:

		/**
		 * Creates a new homography tracker object and uses 31 as patch size.
		 */
		inline HomographyTracker() = default;

		/**
		 * Creates a new homography tracker object and uses 31 as patch size.
		 * @param patchSize The size of the patches, possible values can be [5, 7, 15, 31].
		 */
		explicit inline HomographyTracker(const unsigned int patchSize);

		/**
		 * Returns the size of the patches which are used for tracking.
		 * @return The patch size currently used, 31 by default
		 */
		inline unsigned int patchSize() const;

		/**
		 * Sets or changes the size of the patches which are used for tracking.
		 * @param size The size of the patches, possible values can be [5, 7, 15, 31].
		 */
		inline void setPatchSize(const unsigned int size);

		/**
		 * Tracks a group of given image points from the previous frame to the current frame and determines the corresponding homography afterwards.
		 * In the case this function is invoked for the first time, the current frame is stored for the next function call (as previous frame) and the identity homography is returned.<br>
		 * The resulting homography will transform points defined in the previous frame to points defined in the current frame (pointCurrent = H * pointPrevious).
		 * @param currentFrame The current frame with pixel format as desired, must be valid
		 * @param yPreviousFrame The previous frame as grayscale frame with pixel format FORMAT_Y8, with same frame dimension and pixel origin as the current frame, can be invalid if this function is invoked for the first time
		 * @param randomGenerator Random number generator object
		 * @param previousPositions Image points (e.g., string feature points) located in the previous image that will be used for the determination of the homography, may be empty if this function is invoked for the first time
		 * @param homography The resulting homography
		 * @param worker Optional worker object to distribute the computation
		 * @param frameBorder The size of the area around the frame's border defining a region from which image points will not be used for tracking, with range [0, min(width, height) / 4)
		 * @return True, if a valid homography could be determined
		 */
		bool trackPoints(const Frame& currentFrame, const Frame& yPreviousFrame, RandomGenerator& randomGenerator, const Vectors2& previousPositions, SquareMatrix3& homography, Worker* worker = nullptr, const Scalar frameBorder = Scalar(10));

		/**
		 * Clears the previous pyramid frame of this tracking object.
		 */
		inline void clear();

		/**
		 * Tracks a group of given image points from the previous frame to the current frame and determines the corresponding homography afterwards.
		 * The resulting homography will transform points defined in the previous frame to points defined in the current frame (pointCurrent = H * pointPrevious).
		 * @param yPreviousFrame The previous frame as grayscale frame, with same frame dimension and pixel origin as the previous/current frame, must have pixel format FORMAT_Y8, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame, with same frame type and layer number as 'previousFramePyramid'
		 * @param randomGenerator Random number generator object
		 * @param previousPositions Image points (e.g., string feature points) located in the previous image that will be used for the determination of the homography
		 * @param homography Resulting homography
		 * @param worker Optional worker object to distribute the computation
		 * @param patchSize The size of the patches used for tracking, possible values can be [5, 7, 15, 31]
		 * @return True, if succeeded
		 */
		static bool trackPoints(const Frame& yPreviousFrame, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, RandomGenerator& randomGenerator, const Vectors2& previousPositions, SquareMatrix3& homography, Worker* worker = nullptr, const unsigned int patchSize = 31u);

		/**
		 * Transforms a given set of points to a new set using a given transformation.
		 * This function will calculate result[i] = transformation * points[i].
		 * @param points The points to be transformed
		 * @param transformation The transformation to be used, must be valid
		 * @return Resulting transformed points
		 */
		static inline Vectors2 transformPoints(const Vectors2& points, const SquareMatrix3& transformation);

	private:

		/// Frame pyramid of the current frame.
		CV::FramePyramid currentFramePyramid_;

		/// Frame pyramid of the previous frame.
		CV::FramePyramid previousFramePyramid_;

		/// The size of the image patches used for tracking, possible values can be [5, 7, 15, 31].
		unsigned int patchSize_ = 31u;
};

HomographyTracker::HomographyTracker(const unsigned int patchSize) :
	patchSize_(patchSize)
{
	ocean_assert(patchSize == 5u || patchSize == 7u || patchSize == 15u || patchSize == 31u);
}

inline unsigned int HomographyTracker::patchSize() const
{
	return patchSize_;
}

inline void HomographyTracker::setPatchSize(const unsigned int size)
{
	ocean_assert(size == 5 || size == 7u || size == 15u || size == 31u);

	patchSize_ = size;
}

inline void HomographyTracker::clear()
{
	previousFramePyramid_.clear();
}

inline Vectors2 HomographyTracker::transformPoints(const Vectors2& points, const SquareMatrix3& transformation)
{
	Vectors2 result;
	result.reserve(points.size());

	for (Vectors2::const_iterator i = points.begin(); i != points.end(); ++i)
	{
		result.emplace_back(transformation * *i);
	}

	return result;
}

}

}

#endif // META_OCEAN_TRACKING_HOMOGRAPHY_TRACKER_H
