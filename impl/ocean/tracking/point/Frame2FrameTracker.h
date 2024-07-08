/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_FRAME_2_FRAME_TRACKER_H
#define META_OCEAN_TRACKING_POINT_FRAME_2_FRAME_TRACKER_H

#include "ocean/tracking/point/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements simple functions allowing to track or to determine the camera motion from one frame to another frame.
 * @ingroup tracking
 */
class Frame2FrameTracker
{
	public:

		/**
		 * Determines the camera pose for a current camera frame by tracking image points from the previous frame (for which the corresponding 3D object points are known) to the current frame.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param previousPose The known camera pose for the previous camera frame, must be valid
		 * @param previousFrame The previous camera frame, must be valid
		 * @param currentFrame The current camera frame, must be valid with same frame type as the previous frame
		 * @param previousObjectPoints The 3D object point locations corresponding to the image points in the previous frame, at least 3
		 * @param previousImagePoints The 2D image points defined in the previous camera frame, one for each provided 3D object point
		 * @param distortImagePoints True, to use the distortion parameters of the camera profile
		 * @param maximalOffset The maximal offset of image points between the previous and the current frame in pixel, with range (0, infinity)
		 * @param currentPose The resulting camera pose of the current frame
		 * @param validIndices Optional resulting indices of the provided previous image points that could be tracked to the current frame
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSize The size of the image patches that are used for point tracking, must be odd, with range [1, infinity)
		 */
		template <unsigned int tSize>
		static bool determinePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& previousPose, const Frame& previousFrame, const Frame& currentFrame, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, const bool distortImagePoints, const unsigned int maximalOffset, HomogenousMatrix4& currentPose, Indices32* validIndices = nullptr, Worker* worker = nullptr);
};

template <unsigned int tSize>
bool Frame2FrameTracker::determinePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& previousPose, const Frame& previousFrame, const Frame& currentFrame, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, const bool distortImagePoints, const unsigned int maximalOffset, HomogenousMatrix4& currentPose, Indices32* validIndices, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid() && previousPose.isValid());
	ocean_assert(previousFrame.isValid() && currentFrame.isValid());

	ocean_assert(previousFrame.frameType() == currentFrame.frameType());

	ocean_assert(previousObjectPoints.size() >= 3);
	ocean_assert(previousObjectPoints.size() == previousImagePoints.size());

	Vectors2 copyPreviousImagePoints(previousImagePoints);

	Vectors2 currentImagePoints;
	Indices32 internalValidIndices;
	if (!CV::Advanced::AdvancedMotion<>::trackPointsBidirectionalSubPixelMirroredBorder<7u>(previousFrame, currentFrame, maximalOffset, 2u, copyPreviousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_14641, worker, &internalValidIndices))
	{
		return false;
	}

	if (internalValidIndices.size() < 3)
	{
		return false;
	}

	if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, previousPose, ConstArraySubsetAccessor<Vector3, Index32>(previousObjectPoints, internalValidIndices), ConstArraySubsetAccessor<Vector2, Index32>(currentImagePoints, internalValidIndices), distortImagePoints, currentPose))
	{
		return false;
	}

	if (validIndices)
	{
		*validIndices = std::move(internalValidIndices);
	}

	return true;
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_FRAME_2_FRAME_TRACKER_H
