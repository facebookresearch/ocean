/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_POINT_TRACK_H
#define META_OCEAN_TRACKING_SLAM_POINT_TRACK_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

// Forward declaration.
class PointTrack;

/**
 * Definition of an unordered map mapping object point ids to point tracks.
 * @ingroup trackingslam
 */
using PointTrackMap = std::unordered_map<Index32, PointTrack>;

/**
 * This class implements a point track which stores continuous 2D observations of a 3D object point over consecutive frames.
 * A point track maintains a sequence of 2D image points observed in consecutive frames, starting from a first frame index.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT PointTrack
{
	public:

		/**
		 * Creates a new point track with a single observation.
		 * @param firstFrameIndex The index of the first frame in which the point is observed, with range [0, infinity)
		 * @param imagePoint The 2D image point observation in the first frame
		 */
		inline PointTrack(const Index32 firstFrameIndex, const Vector2& imagePoint);

		/**
		 * Creates a new point track from existing observations.
		 * @param firstFrameIndex The index of the first frame in which the point is observed, with range [0, infinity)
		 * @param imagePoints The 2D image point observations for consecutive frames starting at firstFrameIndex, will be moved, must not be empty
		 */
		explicit inline PointTrack(const Index32 firstFrameIndex, Vectors2&& imagePoints);

		/**
		 * Adds a new observation to this point track.
		 * The observation must be for the next consecutive frame (lastFrameIndex() + 1).
		 * @param frameIndex The index of the frame for which the observation will be added, must be lastFrameIndex() + 1
		 * @param imagePoint The 2D image point observation
		 */
		inline void addObservation(const Index32 frameIndex, const Vector2& imagePoint);

		/**
		 * Returns whether this point track has an observation for a given frame index.
		 * @param frameIndex The index of the frame to check
		 * @param imagePoint Optional resulting 2D image point of the observation, nullptr if not of interest
		 * @return True, if an observation exists for the given frame index
		 */
		inline bool hasObservation(const Index32 frameIndex, Vector2* imagePoint = nullptr) const;

		/**
		 * Returns the observation for a given frame index.
		 * @param frameIndex The index of the frame for which the observation will be returned, with range [firstFrameIndex(), lastFrameIndex()]
		 * @return The 2D image point observation for the given frame
		 */
		inline const Vector2& observation(const Index32 frameIndex) const;

		/**
		 * Returns the last observation of this point track.
		 * @return The 2D image point of the last observation
		 */
		inline const Vector2& lastImagePoint() const;

		/**
		 * Returns the index of the first frame in which the point was observed.
		 * @return The first frame index
		 */
		inline Index32 firstFrameIndex() const;

		/**
		 * Returns the index of the last frame in which the point was observed.
		 * @return The last frame index
		 */
		inline Index32 lastFrameIndex() const;

		/**
		 * Returns the index of the next expected frame for a new observation.
		 * @return The next frame index, which is lastFrameIndex() + 1
		 */
		inline Index32 nextFrameIndex() const;

		/**
		 * Returns all 2D image point observations of this point track.
		 * @return The vector of image points for consecutive frames starting at firstFrameIndex()
		 */
		inline const Vectors2& imagePoints() const;

		/**
		 * Returns the number of observations in this point track.
		 * @return The number of observations, with range [1, infinity)
		 */
		inline size_t numberObservations() const;

		/**
		 * Returns the number of observations up to and including a given frame index.
		 * @param frameIndex The frame index up to which to count observations
		 * @return The number of observations, 0 if the frame index is outside the track's range
		 */
		inline size_t numberObservationsUntil(const Index32 frameIndex) const;

		/**
		 * Returns whether this point track is valid.
		 * A valid point track has a valid first frame index and at least one observation.
		 * @return True, if valid
		 */
		inline bool isValid() const;

		/**
		 * Determines the percentile track length for a set of point tracks up to a given frame index.
		 * @param frameIndex The frame index up to which to consider observations
		 * @param pointTrackMap The map of point tracks to analyze
		 * @param minimalTracks The minimal number of tracks required to compute the percentile, with range [1, infinity)
		 * @param percentile The percentile to compute, with range [0.0, 1.0]
		 * @return The percentile track length, 0 if there are fewer than minimalTracks tracks
		 */
		static size_t determineTracksLengthUntil(const Index32 frameIndex, const PointTrackMap& pointTrackMap, const size_t minimalTracks, const double percentile);

		/**
		 * Extracts 2D-2D correspondences from point tracks that span between two frame indices.
		 * @param firstFrameIndex The index of the first frame
		 * @param lastFrameIndex The index of the last frame, must be > firstFrameIndex
		 * @param pointTrackMap The map of point tracks to extract correspondences from
		 * @param firstImagePoints The resulting image points in the first frame
		 * @param lastImagePoints The resulting corresponding image points in the last frame
		 * @param objectPointIds The resulting object point ids for each correspondence
		 */
		static void extractCorrespondences(const Index32 firstFrameIndex, const Index32 lastFrameIndex, const PointTrackMap& pointTrackMap, Vectors2& firstImagePoints, Vectors2& lastImagePoints, Indices32& objectPointIds);

		/**
		 * Determines the percentile viewing angle between ray directions from two camera poses.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_camera0 The transformation from the first camera to the world, must be valid
		 * @param world_T_camera1 The transformation from the second camera to the world, must be valid
		 * @param imagePoints0 The image points in the first camera frame
		 * @param imagePoints1 The corresponding image points in the second camera frame, must have the same size as imagePoints0
		 * @param validIndices The indices of valid correspondences to consider, must not be empty
		 * @param percentile The percentile to compute, with range [0.0, 1.0]
		 * @return The percentile viewing angle in radians
		 */
		static Scalar determineViewingAngle(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera0, const HomogenousMatrix4& world_T_camera1, const Vectors2& imagePoints0, const Vectors2& imagePoints1, const Indices32& validIndices, const double percentile);

	protected:

		/// The index of the first frame in which the point was observed, -1 if invalid.
		Index32 firstFrameIndex_ = Index32(-1);

		/// The 2D image point observations for consecutive frames starting at firstFrameIndex_.
		Vectors2 imagePoints_;
};

inline PointTrack::PointTrack(const Index32 firstFrameIndex, const Vector2& imagePoint) :
	firstFrameIndex_(firstFrameIndex)
{
	ocean_assert(firstFrameIndex_ != Index32(-1));

	imagePoints_.reserve(32);
	imagePoints_.emplace_back(imagePoint);

	ocean_assert(isValid());
}

inline PointTrack::PointTrack(const Index32 firstFrameIndex, Vectors2&& imagePoints) :
	firstFrameIndex_(firstFrameIndex),
	imagePoints_(std::move(imagePoints))
{
	ocean_assert(isValid());
}

inline void PointTrack::addObservation(const Index32 frameIndex, const Vector2& imagePoint)
{
	ocean_assert(isValid());

	ocean_assert_and_suppress_unused(frameIndex == lastFrameIndex() + 1u, frameIndex);

	imagePoints_.push_back(imagePoint);
}

inline bool PointTrack::hasObservation(const Index32 frameIndex, Vector2* imagePoint) const
{
	ocean_assert(isValid());

	if (frameIndex < firstFrameIndex_ || frameIndex > lastFrameIndex())
	{
		return false;
	}

	if (imagePoint != nullptr)
	{
		*imagePoint = imagePoints_[frameIndex - firstFrameIndex_];
	}

	return true;
}

inline const Vector2& PointTrack::observation(const Index32 frameIndex) const
{
	ocean_assert(isValid());
	ocean_assert(firstFrameIndex_ <= frameIndex && frameIndex <= lastFrameIndex());

	return imagePoints_[frameIndex - firstFrameIndex_];
}

inline const Vector2& PointTrack::lastImagePoint() const
{
	ocean_assert(isValid());

	return imagePoints_.back();
}

inline Index32 PointTrack::firstFrameIndex() const
{
	return firstFrameIndex_;
}

inline Index32 PointTrack::lastFrameIndex() const
{
	ocean_assert(isValid());

	return firstFrameIndex_ + Index32(imagePoints_.size()) - 1u;
}

inline Index32 PointTrack::nextFrameIndex() const
{
	ocean_assert(isValid());

	return firstFrameIndex_ + Index32(imagePoints_.size());
}

inline const Vectors2& PointTrack::imagePoints() const
{
	ocean_assert(isValid());

	return imagePoints_;
}

inline size_t PointTrack::numberObservations() const
{
	ocean_assert(isValid());

	return imagePoints_.size();
}

inline size_t PointTrack::numberObservationsUntil(const Index32 frameIndex) const
{
	ocean_assert(isValid());

	if (firstFrameIndex_ > frameIndex || lastFrameIndex() < frameIndex)
	{
		return 0;
	}

	ocean_assert(firstFrameIndex_ <= frameIndex && frameIndex <= lastFrameIndex());

	return size_t(frameIndex - firstFrameIndex_ + 1u);
}

inline bool PointTrack::isValid() const
{
	ocean_assert(firstFrameIndex_ == Index32(-1) || !imagePoints_.empty());

	return firstFrameIndex_ != Index32(-1);
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_POINT_TRACK_H
