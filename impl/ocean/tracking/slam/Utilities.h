/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_UTILITIES_H
#define META_OCEAN_TRACKING_SLAM_UTILITIES_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/TrackerMono.h"

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements utility functions for SLAM.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT Utilities
{
	public:

		/**
		 * Paints object points and feature tracks into a frame, coloring object points by their localization precision.
		 * @param frame The frame to paint into, must be valid and have FORMAT_RGB24
		 * @param cameraClipper The camera clipper used for projecting 3D points to image coordinates
		 * @param world_T_camera The transformation from camera to world coordinates
		 * @param debugData The debug data containing object points and feature tracks
		 * @param maximalTrackLength The maximum number of track points to draw per track, with range [1, infinity)
		 * @param darkeningFactor Optional factor to darken the frame before painting, 0 to disable, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool paintDebugDataByLocalizationPrecision(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const size_t maximalTrackLength, const unsigned int darkeningFactor = 0u);

		/**
		 * Paints object points and feature tracks into a frame, coloring object points by their pose estimation precision.
		 * @param frame The frame to paint into, must be valid and have FORMAT_RGB24
		 * @param cameraClipper The camera clipper used for projecting 3D points to image coordinates
		 * @param world_T_camera The transformation from camera to world coordinates
		 * @param debugData The debug data containing object points and feature tracks
		 * @param maximalTrackLength The maximum number of track points to draw per track, with range [1, infinity)
		 * @param darkeningFactor Optional factor to darken the frame before painting, 0 to disable, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool paintDebugDataByPosePrecision(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const size_t maximalTrackLength, const unsigned int darkeningFactor = 0u);

		/**
		 * Paints object points into a frame using a uniform green color.
		 * @param frame The frame to paint into, must be valid and have FORMAT_RGB24
		 * @param cameraClipper The camera clipper used for projecting 3D points to image coordinates
		 * @param world_T_camera The transformation from camera to world coordinates
		 * @param debugData The debug data containing object points
		 * @param darkeningFactor Optional factor to darken the frame before painting, 0 to disable, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool paintObjectPoints(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const unsigned int darkeningFactor = 0u);

		/**
		 * Serializes FramesStatistics to a JSON file.
		 * @param framesStatistics The frames statistics to serialize
		 * @param filename The name of the file to write to, must not be empty
		 * @return True, if the serialization succeeded
		 */
		static bool serializeFramesStatisticsToJSON(const TrackerMono::FramesStatistics& framesStatistics, const std::string& filename);

		/**
		 * Deserializes FramesStatistics from a JSON file.
		 * @param filename The name of the file to read from, must not be empty
		 * @param framesStatistics The resulting frames statistics
		 * @return True, if the deserialization succeeded
		 */
		static bool deserializeFramesStatisticsFromJSON(const std::string& filename, TrackerMono::FramesStatistics& framesStatistics);
};

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_UTILITIES_H
