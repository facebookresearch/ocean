/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_TRACKER_STEREO_H
#define META_OCEAN_TRACKING_MAPBUILDING_TRACKER_STEREO_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"

#include "ocean/cv/FramePyramid.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a tracker for mono cameras.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT TrackerStereo final : public RelocalizerStereo
{
	protected:

		/**
		 * Definition of an unordered map mapping object point ids to object point indices.
		 */
		typedef std::unordered_map<Index32, Index32> ObjectPointIdMap;

	public:

		/**
		 * Creates a new tracker object.
		 */
		TrackerStereo() = default;

		/**
		 * Creates a new tracker object.
		 * @param imageFeaturePointDetectorFunction The feature detection and description function to be used
		 */
		explicit TrackerStereo(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Sets or updates the feature map to be used for relocalization.
		 * @see Relocalizer::setFeatureMap().
		 */
		bool setFeatureMap(SharedUnifiedFeatureMap featureMap) override;

		/**
		 * Tracks the current frame a given frame.
		 * @param anyCameraA The camera profile defining the projection of the first camera, must be valid
		 * @param anyCameraB The camera profile defining the projection of the second camera, must be valid
		 * @param device_T_cameraA The transformation between the first camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param device_T_cameraB The transformation between the second camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param yFrameA The first frame with pixel format FORMAT_Y8, must be valid
		 * @param yFrameB The second frame with pixel format FORMAT_Y8, must be valid
		 * @param world_T_device The resulting device pose transforming device to world
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalProjectionError The maximal projection error between 3D object points and their 2D observations, in pixels, with range [0, infinity)
		 * @param world_T_roughDevice Optional rough device pose to speedup the relocalization, if known, invalid otherwise
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		bool track(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Frame& yFrameA, const Frame& yFrameB, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const HomogenousMatrix4& world_T_roughDevice = HomogenousMatrix4(false), Worker* worker = nullptr);

	protected:

		/**
		 * Creates the feature correspondences debugging element.
		 * @param anyCameraA The camera profile defining the projection of the first camera, must be valid
		 * @param anyCameraB The camera profile defining the projection of the second camera, must be valid
		 * @param world_T_device The transformation between device and world, must be valid
		 * @param device_T_cameraA The transformation between the first camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param device_T_cameraB The transformation between the second camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 */
		void createDebuggingElementFeatureCorrespondences(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB);

	protected:

		/// The frame pyramid holding the previous first camera image.
		CV::FramePyramid yPreviousFramePyramidA_;

		/// The frame pyramid holding the previous second camera image.
		CV::FramePyramid yPreviousFramePyramidB_;

		/// The frame pyramid holding the current first camera image.
		CV::FramePyramid yCurrentFramePyramidA_;

		/// The frame pyramid holding the current second camera image.
		CV::FramePyramid yCurrentFramePyramidB_;

		/// The previous image points in the first camera.
		Vectors2 previousImagePointsA_;

		/// The previous image points in the second camera.
		Vectors2 previousImagePointsB_;

		/// The previous object points observed in the first camera.
		Vectors3 previousObjectPointsA_;

		/// The previous object points observed in the second camera.
		Vectors3 previousObjectPointsB_;

		/// The map mapping object point ids to object point indices.
		ObjectPointIdMap objectPointIdMap_;
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_TRACKER_STEREO_H
