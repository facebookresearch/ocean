/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_TRACKING_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_SLAM_TRACKING_CORRESPONDENCES_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/PointTrack.h"
#include "ocean/tracking/slam/Tracker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/GravityConstraints.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class holds 2D-2D point correspondences for frame-to-frame tracking.
 * The class manages image point correspondences between consecutive frames, along with associated 3D object point information for localized points.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT TrackingCorrespondences
{
	public:

		/// Definition of valid correspondence flags.
		using ValidCorrespondences = std::vector<uint8_t>;

	public:

		/**
		 * Creates a new tracking correspondences object with reserved memory.
		 */
		TrackingCorrespondences();

		/**
		 * Updates the internal data structures for a new frame.
		 * This method resets previously stored information and populates the correspondences from the localized object point map and point track map.
		 * @param previousFrameIndex The index of the previous frame, with range [0, infinity)
		 * @param mapVersion The current map version
		 * @param localizedObjectPointMap The map of localized object points
		 * @param pointTrackMap The map of point tracks
		 * @param minimalFrontPrecision The minimal precision for points to be sorted to the front
		 */
		void update(const Index32 previousFrameIndex, const Index32 mapVersion, const LocalizedObjectPointMap& localizedObjectPointMap, PointTrackMap& pointTrackMap, const LocalizedObjectPoint::LocalizationPrecision minimalFrontPrecision);

		/**
		 * Optimizes the previous camera pose using the stored correspondences.
		 * @param camera The camera profile, must be valid
		 * @param world_T_previousCamera The previous camera pose to optimize, must be valid
		 * @param minimalCorrespondences The minimal number of correspondences required
		 * @param world_T_optimizedPreviousCamera The resulting optimized pose
		 * @param estimatorType The robust estimator type
		 * @param gravityConstraints Optional gravity constraints
		 * @return True if optimization succeeded
		 */
		bool optimizePreviousCameraPose(const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const size_t minimalCorrespondences, HomogenousMatrix4& world_T_optimizedPreviousCamera, const Geometry::Estimator::EstimatorType estimatorType, const Geometry::GravityConstraints* gravityConstraints) const;

		/**
		 * Tracks the image points from the previous frame to the current frame.
		 * This method performs bidirectional feature tracking using the stored previous image points and populates the current image points and valid correspondence flags.
		 * @param currentFrameIndex The index of the current frame, with range [1, infinity)
		 * @param camera The camera profile, must be valid
		 * @param world_T_previousCamera The pose of the previous camera in world coordinates, invalid if unknown
		 * @param yPreviousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param yCurrentFramePyramid The frame pyramid of the current frame, must be valid and have the same frame type as the previous pyramid
		 * @param trackingParameters The tracking parameters defining pyramid configuration and patch size
		 * @param previousCamera_Q_currentCamera The rotation from the previous camera to the current camera (from IMU), invalid if unavailable
		 * @param minimalFrontPrecision The minimal precision for object points to be used for guided tracking predictions
		 */
		void trackImagePoints(const Index32 currentFrameIndex, const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, const Tracker::TrackingParameters& trackingParameters, const Quaternion& previousCamera_Q_currentCamera, const LocalizedObjectPoint::LocalizationPrecision minimalFrontPrecision);

		/**
		 * Returns the previous frame index.
		 * @return The previous frame index
		 */
		inline Index32 previousFrameIndex() const;

		/**
		 * Returns the map version.
		 * @return The map version
		 */
		inline Index32 mapVersion() const;

		/**
		 * Returns the previous image points.
		 * @return The previous image points
		 */
		inline const Vectors2& previousImagePoints() const;

		/**
		 * Returns the current image points.
		 * @return The current image points
		 */
		inline const Vectors2& currentImagePoints() const;

		/**
		 * Returns the point IDs.
		 * @return The point IDs
		 */
		inline const Indices32& pointIds() const;

		/**
		 * Returns the valid correspondences flags.
		 * @return The valid correspondences flags
		 */
		inline const ValidCorrespondences& validCorrespondences() const;

		/**
		 * Counts the number of valid correspondences.
		 * @return The number of valid correspondences
		 */
		size_t countValidCorrespondences() const;

		/**
		 * Returns the 3D object points for localized correspondences.
		 * @return The object points
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the localization precisions for localized correspondences.
		 * @return The localization precisions
		 */
		inline const LocalizedObjectPoint::LocalizationPrecisions& objectPointPrecisions() const;

		/**
		 * Returns the number of correspondences.
		 * @return The number of correspondences
		 */
		inline size_t size() const;

		/**
		 * Returns whether there are no correspondences.
		 * @return True if empty
		 */
		inline bool isEmpty() const;

	protected:

		/// The index of the previous frame.
		Index32 previousFrameIndex_ = Index32(-1);

		/// The map version at the time the correspondences were gathered.
		Index32 mapVersion_ = 0u;

		/// The 2D image points in the previous frame.
		Vectors2 previousImagePoints_;

		/// The 2D image points in the current frame, one for each previous image point.
		Vectors2 currentImagePoints_;

		/// The unique IDs of the points, one for each previous image point.
		Indices32 pointIds_;

		/// Set of point ids for fast lookup, workaround for separation between localized object points and point tracks.
		UnorderedIndexSet32 pointIdSet_;

		/// Flags indicating which correspondences are valid after tracking.
		ValidCorrespondences validCorrespondences_;

		/// The 3D object points for localized correspondences.
		Vectors3 objectPoints_;

		/// The localization precisions for localized correspondences.
		LocalizedObjectPoint::LocalizationPrecisions objectPointPrecisions_;

		// TODO add whether object point has descriptor (to ensure that we can switch from TS_INITIALIZING to TS_TRACKING)
};

inline Index32 TrackingCorrespondences::previousFrameIndex() const
{
	return previousFrameIndex_;
}

inline Index32 TrackingCorrespondences::mapVersion() const
{
	return mapVersion_;
}

inline const Vectors2& TrackingCorrespondences::previousImagePoints() const
{
	return previousImagePoints_;
}

inline const Vectors2& TrackingCorrespondences::currentImagePoints() const
{
	return currentImagePoints_;
}

inline const Indices32& TrackingCorrespondences::pointIds() const
{
	return pointIds_;
}

inline const TrackingCorrespondences::ValidCorrespondences& TrackingCorrespondences::validCorrespondences() const
{
	return validCorrespondences_;
}

inline const Vectors3& TrackingCorrespondences::objectPoints() const
{
	return objectPoints_;
}

inline const LocalizedObjectPoint::LocalizationPrecisions& TrackingCorrespondences::objectPointPrecisions() const
{
	return objectPointPrecisions_;
}

inline size_t TrackingCorrespondences::size() const
{
	ocean_assert(previousImagePoints_.size() == pointIds_.size());

	return pointIds_.size();
}

inline bool TrackingCorrespondences::isEmpty() const
{
	return pointIds_.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_TRACKING_CORRESPONDENCES_H
