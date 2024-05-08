/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_CONTOUR_TRACKER_H
#define META_OCEAN_CV_SEGMENTATION_CONTOUR_TRACKER_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements a contour tracker able to detect an object and to track the object's contour within a video sequence with realtime performance.
 * The tracker supports planar backgrounds as well as slightly non-planar backgrounds and can handle changing contour shapes to some extend.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT ContourTracker
{
	public:

		/**
		 * Creates a new tracker object.
		 */
		ContourTracker() = default;

		/**
		 * Returns the dense contour of the most recent detection or tracking interaction.
		 * @return The object's dense contour, an empty contour if no object has been detected or tracked
		 */
		inline const PixelContour& denseContour() const;

		/**
		 * Returns the dense contour of the most recent detection or tracking interaction with sub-pixel accuracy.
		 * @return The object's dense contour, an empty contour if no object has been detected or tracked
		 */
		inline const Vectors2& denseContourSubPixel() const;

		/**
		 * Returns the most dominant homography which has been determined during the most recent tracking iteration.
		 * The homography will transform points defined in the previous frame to points defined in the current frame (pointCurrent = H * pointPrevious).
		 * @return The most recent homography
		 */
		inline const SquareMatrix3& homography() const;

		/**
		 * Re-detects an object (the object's contour respectively) in a given frame within a region enclosed by a rough contour.
		 * @param frame The frame in which the frame is visible, must be valid
		 * @param roughContour The rough contour roughly enclosing the object to be detected, with at least three locations
		 * @param randomGenerator Random number generator object
		 * @param extraContourOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param yFrame Optional Y8 frame of the given frame to speed up the computation, with same frame dimension and pixel origin
		 * @return True, if succeeded
		 */
		bool detectObject(const Frame& frame, const PixelContour& roughContour, RandomGenerator& randomGenerator, const unsigned int extraContourOffset, Worker* worker = nullptr, const Frame& yFrame = Frame());

		/**
		 * Tracks an already detected contour (around an object) from the previous frame to the current frame.
		 * @param frame The current frame for which the new contour will be determined, with same frame type as used for the detection or a previous tracking iteration, must be valid
		 * @param randomGenerator Random number generator object
		 * @param extraContourOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param yFrame Optional Y8 frame of the given frame to speed up the computation, with same frame dimension and pixel origin
		 * @return True, if succeeded
		 */
		bool trackObject(const Frame& frame, RandomGenerator& randomGenerator, const unsigned int extraContourOffset, Worker* worker = nullptr, const Frame& yFrame = Frame());

		/**
		 * Clears all resources and resets the tracker.
		 */
		void clear();

	private:

		/// The frame pyramid of the previous frame.
		FramePyramid previousFramePyramid_;

		/// The frame pyramid of the current frame.
		FramePyramid currentFramePyramid_;

		/// The contour belonging to the most recent (previous) frame.
		PixelContour previousDenseContour_;

		/// The contour belonging to the most recent (previous) frame with sub-pixel accuracy.
		Vectors2 previousDenseContourSubPixel_;

		/// The strongest (feature) points of the contour of the previous frame.
		Vectors2 previousContourStrongest_;

		/// The homography which has been determine during the previous tracking iteration.
		SquareMatrix3 previousHomography_ = SquareMatrix3(true);

		/// An intermediate rough mask frame.
		Frame intermediateRoughMask_;

		/// True, if the tracker should try to invoke a planar tracker; False, if the tracker should use a more generous approach.
		bool usePlanarTracking_ = false;
};

inline const PixelContour& ContourTracker::denseContour() const
{
	return previousDenseContour_;
}

inline const Vectors2& ContourTracker::denseContourSubPixel() const
{
	return previousDenseContourSubPixel_;
}

inline const SquareMatrix3& ContourTracker::homography() const
{
	return previousHomography_;
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_CONTOUR_TRACKER_H
