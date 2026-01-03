/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_SLAM_DEBUG_ELEMENTS_H
#define META_OCEAN_TRACKING_SLAM_SLAM_DEBUG_ELEMENTS_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/CameraPose.h"
#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/OccupancyArray.h"
#include "ocean/tracking/slam/PointTrack.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Singleton.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * The class extends DebugElements to provide visual debugging output for various SLAM components such as occupancy arrays, tracked image points, object points, and overall tracking results.
 * Debug elements are only generated when the corresponding element is activated via the base class.
 * @see DebugElements
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT SLAMDebugElements :
	public DebugElements,
	public Singleton<SLAMDebugElements>
{
	friend class Singleton<SLAMDebugElements>;

	public:

		/// True, to allow debugging and enable debug element generation; False, to disable debugging globally.
		static constexpr bool allowDebugging_ = false;

		/**
		 * Definition of element identifiers for debug visualizations.
		 */
		enum ElementId : uint32_t
		{
			/// An invalid element id.
			EI_INVALID = 0u,
			/// The id of the debug element for the occupancy array visualization.
			EI_OCCUPANCY_ARRAY,
			/// The id of the debug element for tracked image point correspondences.
			EI_TRACKED_IMAGE_POINTS,
			/// The id of the debug element for newly detected features.
			EI_NEW_FEATURES,
			/// The id of the debug element for 3D object point projections.
			EI_OBJECT_POINTS,
			/// The id of the debug element for 2D image point tracks.
			EI_IMAGE_POINTS,
			/// The id of the debug element for the comprehensive SLAM tracking result.
			EI_RESULT
		};

		/**
		 * Sets the angle of the output rotation.
		 * @param angle The angle of the output rotation, in degrees. Must be a multiple of 90.
		 */
		inline void setOutputRotation(const int angle);

	public:

		/**
		 * Updates the debug element based on the occupancy array.
		 * The visualization overlays the occupancy state on the input frame.<br>
		 * Empty bins are darkened (reduced brightness), while occupied bins are highlighted in green.<br>
		 * Displays the coverage percentage and whether more feature points are needed.
		 * @param yFrame The frame for which the occupancy array has been created, must be valid
		 * @param occupancyArray The occupancy array to visualize, must be valid
		 */
		void updateOccupancyArray(const Frame& yFrame, const OccupancyArray& occupancyArray);

		/**
		 * Updates the debug element based on tracked image point correspondences.
		 * The visualization draws lines between previous and current image points to show optical flow.<br>
		 * Valid correspondences are drawn in green, invalid correspondences are drawn in red.
		 * @param yFrame The frame for which the tracking was performed, must be valid
		 * @param previousImagePoints The image points from the previous frame, can be nullptr if numberCorrespondences is 0
		 * @param currentImagePoints The corresponding image points in the current frame, can be nullptr if numberCorrespondences is 0
		 * @param validCorrespondences Array indicating valid (1) or invalid (0) correspondences, can be nullptr if numberCorrespondences is 0
		 * @param numberCorrespondences The number of point correspondences, with range [0, infinity)
		 */
		void updateTrackedImagePoints(const Frame& yFrame, const Vector2* previousImagePoints, const Vector2* currentImagePoints, uint8_t* validCorrespondences, const size_t numberCorrespondences);

		/**
		 * Updates the debug element based on projected 3D object points.
		 * The visualization projects localized 3D object points onto the 2D frame.<br>
		 * Object points observed in the current frame are drawn in blue, while other visible points (when showAll is true) are drawn in red.
		 * @param yFrame The frame onto which the object points will be projected, must be valid
		 * @param camera The camera profile to use for projection, must be valid
		 * @param cameraPose The camera pose to use for projection
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized 3D object points to visualize
		 * @param showAll True, to show all visible object points; False, to show only tracked object points
		 */
		void updateObjectPoints(const Frame& yFrame, const AnyCamera& camera, const CameraPose& cameraPose, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const bool showAll = true);

		/**
		 * Updates the debug element based on 2D image point tracks.
		 * The visualization shows the trajectory of tracked points across frames.<br>
		 * New points (with only one observation) are drawn as yellow dots, while tracked points with history are connected with green lines.
		 * @param yFrame The frame for which the point tracks are visualized, must be valid
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param pointTrackMap The map of point tracks to visualize
		 */
		void updateImagePoints(const Frame& yFrame, const Index32 frameIndex, const PointTrackMap& pointTrackMap);

		/**
		 * Updates the debug element based on the comprehensive SLAM tracking result.
		 * The visualization combines multiple elements: 2D point tracks with trajectory lines, 3D-2D correspondences with reprojection quality indicators, and optional triangle mesh visualization.<br>
		 * New 2D points are drawn as yellow dots, tracked points are connected with green lines.<br>
		 * 3D-2D correspondences are drawn with color-coded reprojection error indicators.
		 * @param yFrame The frame for which the tracking was performed, must be valid
		 * @param camera The camera profile used for projection, must be valid
		 * @param cameraPose The SLAM camera pose; if invalid, only 2D tracking information is visualized
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param pointTrackMap The map of 2D point tracks to visualize
		 * @param localizedObjectPointMap The map of localized 3D object points to visualize
		 * @param objectTriangles Optional array of 3D triangles to visualize as mesh, nullptr if not used
		 * @param numberObjectTriangles The number of triangles in the objectTriangles array, with range [0, infinity)
		 */
		void updateResult(const Frame& yFrame, const AnyCamera& camera, const CameraPose& cameraPose, const Index32 frameIndex, const PointTrackMap& pointTrackMap, const LocalizedObjectPointMap& localizedObjectPointMap, const Triangle3* objectTriangles, const size_t numberObjectTriangles);

	protected:

		/// The angle of the output rotation, in degrees.
		int outputRotationAngle_ = 0;
};

inline void SLAMDebugElements::setOutputRotation(const int angle)
{
	ocean_assert(angle % 90 == 0);
	outputRotationAngle_ = angle;
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_SLAM_DEBUG_ELEMENTS_H
