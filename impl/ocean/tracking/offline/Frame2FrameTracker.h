/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_FRAME_2_FRAME_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_FRAME_2_FRAME_TRACKER_H

#include "ocean/tracking/offline/Offline.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/Utilities.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

/**
 * This class implements a tracker allowing to tracker feature points between two frames.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT Frame2FrameTracker
{
	public:

		/**
		 * This function tracks a planar plane or object between two successive frames.
		 * @param pinholeCamera The pinhole camera object defining the projection
		 * @param previousPose Pose of the previous frame
		 * @param previousPlane 3D plane that specifies the planar object in the previous frame
		 * @param previousImagePoints Image points located in the previous frame all lying on a 3D plane
		 * @param nextImagePoints Image points located in the next frame, each point corresponds to a point in the previous frame, by index
		 * @param correspondences The number of points correspondences
		 * @param nextPose Resulting pose for the next frame
		 * @return True, if succeeded
		 */
		static bool trackPlanarObject(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const Vector2* previousImagePoints, const Vector2* nextImagePoints, const size_t correspondences, HomogenousMatrix4& nextPose);

		/**
		 * This function tracks a planar plane or object between two successive frames.
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * @param pinholeCamera The pinhole camera object defining the projection
		 * @param previousPyramid Previous frame pyramid
		 * @param nextPyramid Next frame pyramid, with same frame type as the previous frame
		 * @param layerRadius Search radius in each layer
		 * @param previousPose Pose of the previous frame
		 * @param previousPlane 3D plane that specifies the planar object in the previous frame
		 * @param previousSubRegion Optional 2D triangles specifying a sub-region in that the points are tracked only
		 * @param nextPose Resulting pose for the next frame
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param strength Minimal strength parameter of the tracked feature points
		 * @param worker Optional worker object to distribute the computation
		 * @param trackingLayers Number of pyramid layers on which points are tracked
		 * @param previousImagePoints Optional resulting image points of the previous image that have been used for tracking
		 * @param currentImagePoints Optional resulting image points of the current image that have been used for tracking, each point corresponds to one point in the previous image
		 * @return The number of feature points that have been used for pose determination, with range [0] or [5, infinity)
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tSize>
		static size_t trackPlanarObject(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int layerRadius, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const CV::SubRegion& previousSubRegion, HomogenousMatrix4& nextPose, const Scalar maximalSqrError /*= Scalar(0.9 * 0.9)*/, const unsigned int horizontalBins /*= 0u*/, const unsigned int verticalBins /*= 0u*/, const unsigned int strength = 30u, Worker* worker = nullptr, const unsigned int trackingLayers = 1u, Vectors2* previousImagePoints = nullptr, Vectors2* currentImagePoints = nullptr);

		/**
		 * This function tracks a planar plane or object between two successive frames.
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * @param pinholeCamera The pinhole camera object defining the projection
		 * @param previousFrame Previous frame
		 * @param nextFrame Next frame, with same frame type as the previous frame
		 * @param layerRadius Search radius in each layer
		 * @param previousPose Pose of the previous frame
		 * @param previousPlane 3D plane that specifies the planar object in the previous frame
		 * @param previousSubRegion Optional sub-region in that the points are tracked only
		 * @param nextPose Resulting pose for the next frame
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param worker Optional worker object to distribute the computation
		 * @return The number of feature points that have been used for pose determination, with range [0] or [5, infinity)
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tSize>
		static inline size_t trackPlanarObject(const PinholeCamera& pinholeCamera, const Frame& previousFrame, const Frame& nextFrame, const unsigned int layerRadius, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const CV::SubRegion& previousSubRegion, HomogenousMatrix4& nextPose, const Scalar maximalSqrError = Scalar(0.9 * 0.9), const CV::FramePyramid::DownsamplingMode downsamplingMode = CV::FramePyramid::DM_FILTER_14641, Worker* worker = nullptr);
};

template <unsigned int tSize>
size_t Frame2FrameTracker::trackPlanarObject(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int layerRadius, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const CV::SubRegion& previousSubRegion, HomogenousMatrix4& nextPose, const Scalar maximalSqrError, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker, const unsigned int trackingLayers, Vectors2* previousImagePoints, Vectors2* currentImagePoints)
{
	ocean_assert(pinholeCamera && previousPyramid && nextPyramid);
	ocean_assert(previousPose.isValid());
	ocean_assert(previousPlane.isValid());

	Vectors2 previousRobustImagePoints, nextRobustImagePoints;
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<tSize>(previousPyramid, nextPyramid, layerRadius, previousRobustImagePoints, nextRobustImagePoints, maximalSqrError, previousSubRegion, horizontalBins, verticalBins, strength, worker, trackingLayers) || previousRobustImagePoints.size() < 5)
	{
		return false;
	}

	ocean_assert(previousRobustImagePoints.size() == nextRobustImagePoints.size());

	if (!trackPlanarObject(pinholeCamera, previousPose, previousPlane, previousRobustImagePoints.data(), nextRobustImagePoints.data(), previousRobustImagePoints.size(), nextPose))
	{
		return 0;
	}

	ocean_assert(previousRobustImagePoints.size() >= 5);
	const size_t result = previousRobustImagePoints.size();

	if (previousImagePoints)
	{
		*previousImagePoints = std::move(previousRobustImagePoints);
	}

	if (currentImagePoints)
	{
		*currentImagePoints = std::move(nextRobustImagePoints);
	}

	return result;
}

template <unsigned int tSize>
size_t Frame2FrameTracker::trackPlanarObject(const PinholeCamera& pinholeCamera, const Frame& previousFrame, const Frame& nextFrame, const unsigned int layerRadius, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const CV::SubRegion& previousSubRegion, HomogenousMatrix4& nextPose, const Scalar maximalSqrError, const CV::FramePyramid::DownsamplingMode downsamplingMode, Worker* worker)
{
	ocean_assert(pinholeCamera && previousFrame && nextFrame);
	ocean_assert(previousPose.isValid());

	if (!previousFrame || previousFrame.frameType() != nextFrame.frameType())
	{
		return false;
	}

	const unsigned int layers = CV::FramePyramid::idealLayers(previousFrame.width(), previousFrame.height(), 60u, 60u, 2u, 64u);
	if (layers == 0u)
	{
		return false;
	}

	const CV::FramePyramid previousPyramid(previousFrame, downsamplingMode, layers, false /*copyFirstLayer*/, worker);
	const CV::FramePyramid nextPyramid(nextFrame, downsamplingMode, layers, false /*copyFirstLayer*/, worker);

	return trackPlanarObject<tSize>(pinholeCamera, previousPyramid, nextPyramid, layerRadius, previousPose, previousPlane, previousSubRegion, nextPose, maximalSqrError, worker);
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_FRAME_2_FRAME_TRACKER_H
