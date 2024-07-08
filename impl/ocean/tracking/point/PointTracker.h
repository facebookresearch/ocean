/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_POINT_TRACKER_H
#define META_OCEAN_TRACKING_POINT_POINT_TRACKER_H

#include "ocean/tracking/point/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Vector2.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements a point tracker able to track points between concurrent frames and stores previous points in a database.
 * The tracker determines feature points in the very first frame - and stored as object points (feature points).<br>
 * Those points are tracked from one frame to another frame as long as possible.<br>
 * All points are tracked from the previous frame to the current frame (and not from a common reference frame to the current frame).<br>
 * Whenever an object point (feature point) is lost the tracker will add a new feature point in the empty region - so that the tracker is always tracking a high number of feature points.<br>
 * Tracking is based on matches between small image patches around the image points.
 * @ingroup trackingpoint
 */
class OCEAN_TRACKING_POINT_EXPORT PointTracker
{
	public:

		/**
		 * Definition of an invalid frame index.
		 */
		static constexpr Index32 invalidFrameIndex = Index32(-1);

		/**
		 * Definition of a vector holding 2D vectors.
		 */
		typedef std::vector<Vectors2> PointTracks;

		/**
		 * Definition of individual tracking modes.
		 */
		enum TrackingMode : uint32_t
		{
			/// Sum square differences tracking with 7 pixel image patch
			TM_SSD_7,
			/// Sum square differences tracking with 15 pixel image patch
			TM_SSD_15,
			/// Sum square differences tracking with 31 pixel image patch
			TM_SSD_31,

			/// Zero-mean sum square differences tracking with 7 pixel image patch
			TM_ZM_SSD_7,
			/// Zero-mean sum square differences tracking with 15 pixel image patch
			TM_ZM_SSD_15,
			/// Zero-mean sum square differences tracking with 31 pixel image patch
			TM_ZM_SSD_31,

			/// End mode.
			TM_END
		};

	public:

		/**
		 * Creates a new point tracker.
		 */
		PointTracker();

		/**
		 * Move constructor.
		 * @param pointTracker The tracker object to be moved
		 */
		PointTracker(PointTracker&& pointTracker) noexcept;

		/**
		 * Copy constructor.
		 * @param pointTracker The tracker object to be copied
		 */
		PointTracker(const PointTracker& pointTracker);

		/**
		 * Sets or changes the tracking mode.
		 * @param trackingMode The new tracking mode
		 */
		inline void setTrackingMode(const TrackingMode trackingMode);

		/**
		 * Returns the current tracking mode.
		 * @return The point tracker's tracking mode
		 */
		inline TrackingMode trackingMode() const;

		/**
		 * Tracks object points (feature points) in a new frame.
		 * @param yFrame The new frame in which the points will be tracked, must have pixel format FORMAT_Y8, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return The index of the new frame, 'invalidFrameIndex' if something went wrong
		 */
		Index32 newFrame(const Frame& yFrame, Worker* worker = nullptr);

		/**
		 * Returns the point tracks for all object points.
		 * One track is a connected path of images points belonging to one object point (feature point).
		 * @param imageIndex The index of the frame for which the tracks will be provided
		 * @param maximalLength The maximal length of each track (the maximal number of image points for each object point), with range [1, infinity)
		 * @return The tracks of tracked object points starting at the specified frame (and going into the past)
		 */
		PointTracks pointTracks(const Index32 imageIndex, const unsigned int maximalLength = (unsigned int)(-1));

		/**
		 * Removes all entries from the tracking database older than a specified frame index.
		 * @param frameIndex The index of the frame which will be the first frame in the database for which data exists
		 * @see clear().
		 */
		void clearUpTo(const unsigned int frameIndex);

		/**
		 * Clears the database containing the object points and their corresponding image points.
		 * @see clearUpTo().
		 */
		inline void clear();

		/**
		 * Returns the internal database storing the topology of the tracked points.
		 * Beware: This function is not thread-safe.
		 * @return The tracker's database
		 */
		inline Database& database();

		/**
		 * Move operator.
		 * @param pointTracker The tracker object to be moved
		 * @return Reference to this object
		 */
		PointTracker& operator=(PointTracker&& pointTracker) noexcept;

		/**
		 * Assign operator.
		 * @param pointTracker The tracker object to be moved
		 * @return Reference to this object
		 */
		PointTracker& operator=(const PointTracker& pointTracker);

	protected:

		/**
		 * Detects new feature points in empty regions of the provided frame.
		 * @param yFrame The provided frame in which new feature points will be detected, must have pixel format FORMAT_Y8, must be valid
		 * @param occupancyArray The occupancy array specifying empty and non-empty regions in the frame, must be valid
		 * @param newFeaturePoints The resulting new feature points
		 * @param worker Optional worker object to distribute the computation
		 */
		void detectNewFeaturePoints(const Frame& yFrame, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Vectors2& newFeaturePoints, Worker* worker = nullptr);

		/**
		 * Tracks feature points from one frame (pyramid) to another frame (pyramid).
		 * @param trackingMode The tracking mode to be used, must be valid
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame, must be valid
		 * @param previousImagePoints The image points located in the previous frame which will be tracked to the current frame, at least one point
		 * @param currentImagePoints The resulting image points located in the current image, one for each previous image points
		 * @param validIndices The indices of all point correspondences that could be tracked reliably (all other correspondences are invalid)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool trackFeaturePoints(const TrackingMode trackingMode, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker);

	protected:

		/// The tracking mode to be used.
		TrackingMode trackingMode_ = TM_ZM_SSD_7;

		/// The database storing the object points (feature points) and their corresponding image points.
		Database database_;

		/// The frame pyramid of the previous frame.
		CV::FramePyramid previousFramePyramid_;

		/// The frame pyramid of the current frame.
		CV::FramePyramid currentFramePyramid_;

		/// The index of the previous frame.
		Index32 previousFrameIndex_ = invalidFrameIndex;

		/// Threshold for strengths of feature points, with range [1, 255]
		unsigned int featurePointStrengthThreshold_ = 15u;

		/// The size of each bin (edge length) in pixel controlling whether new feature points will be added in an empty region.
		unsigned int binSize_ = 40u;

		/// The lock for this tracker.
		mutable Lock lock_;
};

inline void PointTracker::setTrackingMode(const TrackingMode trackingMode)
{
	const ScopedLock scopedLock(lock_);

	trackingMode_ = trackingMode;
}

inline PointTracker::TrackingMode PointTracker::trackingMode() const
{
	const ScopedLock scopedLock(lock_);

	return trackingMode_;
}

inline void PointTracker::clear()
{
	const ScopedLock scopedLock(lock_);

	database_.clear<false>();

	previousFrameIndex_ = invalidFrameIndex;
	previousFramePyramid_.clear();
	currentFramePyramid_.clear();
}

inline Database& PointTracker::database()
{
	return database_;
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_POINT_TRACKER_H
