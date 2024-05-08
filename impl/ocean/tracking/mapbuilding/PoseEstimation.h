/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_H
#define META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements functions to estimate camera poses using unified data structures.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT PoseEstimation
{
	public:

		/**
		 * Determines the 6-DOF pose of a mono camera based on 2D/3D correspondences using unguided and guided matching based an octree and descriptor trees.
		 * @param anyCamera The camera profile to be used, must be valid
		 * @param unifiedUnguidedMatching The unified data structure holding the information for an unguided feature matching, must be valid
		 * @param unifiedGuidedMatching The unified data structure holding the information for a guided feature matching, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting camera pose transforming camera to world, with default camera looking into the negative z-space an y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error between a projected 3D object point and a 2D image point so that both points count as corresponding, in pixel, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param usedObjectPointIds Optional resulting ids of the 3D object points which have been used to determine the camera pose, nullptr if not of interest
		 * @param usedImagePointIndices Optional resulting indices of the 2D image points which have been used to determine the camera pose, nullptr if not of interest
		 * @param world_T_roughCamera Optional known rough camera pose allowing to skip the unguided matching, invalid if unknown
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determinePose(const AnyCamera& anyCamera, const UnifiedUnguidedMatching& unifiedUnguidedMatching, const UnifiedGuidedMatching& unifiedGuidedMatching, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), Indices32* usedObjectPointIds = nullptr, Indices32* usedImagePointIndices = nullptr, const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false), Worker* worker = nullptr);

		/**
		 * Determines the 6-DOF pose of a stereo camera based on for 2D/3D correspondences using unguided and guided matching based an octree and descriptor trees.
		 * @param anyCameraA The camera profile of the first stereo camera to be used, must be valid
		 * @param anyCameraB The camera profile of the second stereo camera to be used, must be valid
		 * @param device_T_cameraA The transformation between the first camera and the device, with default camera pointing towards the negative z-space with y-axis up, must be valid
		 * @param device_T_cameraB The transformation between the second camera and the device, with default camera pointing towards the negative z-space with y-axis up, must be valid
		 * @param unifiedUnguidedMatchingA The unified data structure holding the information for an unguided feature matching for the first frame, must be valid
		 * @param unifiedUnguidedMatchingB The unified data structure holding the information for an unguided feature matching for the second frame, must be valid
		 * @param unifiedGuidedMatchingA The unified data structure holding the information for a guided feature matching for the first frame, must be valid
		 * @param unifiedGuidedMatchingB The unified data structure holding the information for a guided feature matching for the second frame, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param world_T_device The resulting device pose transforming device to world
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error between a projected 3D object point and a 2D image point so that both points count as corresponding, in pixel, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param usedObjectPointIdsA Optional resulting ids of the 3D object points which have been used to determine the camera pose in the first frame, nullptr if not of interest
		 * @param usedObjectPointIdsB Optional resulting ids of the 3D object points which have been used to determine the camera pose in the second frame, nullptr if not of interest
		 * @param usedImagePointIndicesA Optional resulting indices of the 2D image points which have been used to determine the camera pose in the first frame, nullptr if not of interest
		 * @param usedImagePointIndicesB Optional resulting indices of the 2D image points which have been used to determine the camera pose in the second frame, nullptr if not of interest
		 * @param world_T_roughDevice Optional known rough device pose allowing to skip the unguided matching, invalid if unknown
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determinePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const UnifiedUnguidedMatching& unifiedUnguidedMatchingA, const UnifiedUnguidedMatching& unifiedUnguidedMatchingB, const UnifiedGuidedMatching& unifiedGuidedMatchingA, const UnifiedGuidedMatching& unifiedGuidedMatchingB, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), Indices32* usedObjectPointIdsA = nullptr, Indices32* usedObjectPointIdsB = nullptr, Indices32* usedImagePointIndicesA = nullptr, Indices32* usedImagePointIndicesB = nullptr, const HomogenousMatrix4& world_T_roughDevice = HomogenousMatrix4(false), Worker* worker = nullptr);
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_H
