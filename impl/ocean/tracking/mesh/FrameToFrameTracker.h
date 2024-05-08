/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MESH_FRAME_TO_FRAME_TRACKER_H
#define META_OCEAN_TRACKING_MESH_FRAME_TO_FRAME_TRACKER_H

#include "ocean/tracking/mesh/Mesh.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

/**
 * This class implements functions for efficient point-based tracking and pose estimation between two camera frames.
 * @ingroup trackingmesh
 */
class OCEAN_TRACKING_MESH_EXPORT FrameToFrameTracker
{
	public:

		/**
		 * Given a set of tracked object and image points from a previous frame, finds the associated
		 * points in the current frame. The pose, 3D object points, and 2D image points are all tracked
		 * and updated internally by this function.
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param previousFramePyramid The image pyramid for the previous camera frame, must be valid
		 * @param currentFramePyramid The image pyramid for the current camera frame, must be valid with same frame type as the previous frame
		 * @param quaternion_previousFrame_T_currentFrame Prior on the rotation from the current frame to the previous frame; may be invalid
		 * @param pose_world_T_previousFrame Estimated camera-to-world, "non-flipped" pose of the previous frame; if the tracking is successful, this value is updated to the estimated pose (wTc) of the current frame
		 * @param previousObjectPoints Set of tracked 3D object points in the previous frame
		 * @param previousImagePoints Set of tracked 2D object points in the previous frame
		 * @param pose_world_T_currentFrame Estimated camera-to-world, "non-flipped" pose of the current frame, invalid if the tracking fails
		 * @param currentObjectPoints Subset of `previousObjectPoints` that were successfully tracked into the current frame
		 * @param currentImagePoints Locations in the current frame of the 2D points that were successfully tracked from the previous frame; the number of output image points equals the number of output object points
		 * @param validIndices Output indices in previousObjectPoints and previousImagePoints corresponding to the tracked points in the current frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the tracking was successful
		 */
		static bool track(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Quaternion& quaternion_previousFrame_T_currentFrame, const HomogenousMatrix4& pose_world_T_previousFrame, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, HomogenousMatrix4& pose_world_T_currentFrame, Vectors3& currentObjectPoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker = nullptr);
};

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_FRAME_TO_FRAME_TRACKER_H
