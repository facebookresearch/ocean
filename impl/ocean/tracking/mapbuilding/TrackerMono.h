/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_TRACKER_MONO_H
#define META_OCEAN_TRACKING_MAPBUILDING_TRACKER_MONO_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/RelocalizerMono.h"

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
class OCEAN_TRACKING_MAPBUILDING_EXPORT TrackerMono final : public RelocalizerMono
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
		TrackerMono() = default;

		/**
		 * Creates a new tracker object.
		 * @param imageFeaturePointDetectorFunction The feature detection and description function to be used
		 */
		explicit TrackerMono(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Sets or updates the feature map to be used for relocalization.
		 * @see Relocalizer::setFeatureMap().
		 */
		bool setFeatureMap(SharedUnifiedFeatureMap featureMap) override;

		/**
		 * Tracks the current frame a given frame.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param yFrame The frame with pixel format FORMAT_Y8, must be valid
		 * @param world_T_camera The resulting camera pose transforming camera to world, with default camera pose pointing towards the negative z-space and y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalProjectionError The maximal projection error between 3D object points and their 2D observations, in pixels, with range [0, infinity)
		 * @param world_T_roughCamera Optional rough camera pose to speedup the relocalization, if known, invalid otherwise
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		bool track(const AnyCamera& anyCamera, const Frame& yFrame, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false), Worker* worker = nullptr);

		protected:

		/**
		 * Creates the feature correspondences debugging element.
		 * @param anyCamera The camera profile defining the projection of the camera, must be valid
		 * @param world_T_camera The transformation between camera and world, must be valid
		 */
		void createDebuggingElementFeatureCorrespondences(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera);

	protected:

		/// The frame pyramid holding the previous camera image.
		CV::FramePyramid yPreviousFramePyramid_;

		/// The frame pyramid holding the current camera image.
		CV::FramePyramid yCurrentFramePyramid_;

		/// The previous image points.
		Vectors2 previousImagePoints_;

		/// The previous object points.
		Vectors3 previousObjectPoints_;

		/// The map mapping object point ids to object point indices.
		ObjectPointIdMap objectPointIdMap_;
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_TRACKER_MONO_H
