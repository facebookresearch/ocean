/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SIMILARITYTRACKER_SIMILARITY_TRACKER_WRAPPER_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SIMILARITYTRACKER_SIMILARITY_TRACKER_WRAPPER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/tracking/point/SimilarityTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingsimilaritytracker Similarity Tracker
 * @{
 * The demo application shows the capabilities of Ocean's Similarity Tracker.<br>
 * The implementation of the basic image aligner functionality is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent wrapper for the similarity tracker which will be used/shared by/across platform specific applications.
 * Beware: You must not have more than one instance within one application.
 * @ingroup applicationdemotrackingsimilaritytracker
 */
class SimilarityTrackerWrapper
{
	public:

		/**
		 * Creates an invalid aligner object.
		 */
		SimilarityTrackerWrapper();

		/**
		 * Move constructor.
		 * @param similarityTrackerWrapper The tracker object to be moved
		 */
		SimilarityTrackerWrapper(SimilarityTrackerWrapper&& similarityTrackerWrapper);

		/**
		 * Creates a new aligner object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of the aligner
		 */
		explicit SimilarityTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a tracker object.
		 */
		~SimilarityTrackerWrapper();

		/**
		 * Explicitly releases this aligner object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking update for the frame.
		 * @param frame The resulting frame showing the blending between the current video frame and the previous frame, may be invalid if the tracker fails
		 * @param time The time the aligner needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param newObjectPosition An optional position within the video frame for which a new tracking region will be defined
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @param trackerConfidence Optional resulting confidence of the similarity tracker
		 * @param regionTextureness Optional resulting textureness of the region of interest of the similarity tracker
		 * @return True, if a new frame was available
		 */
		bool trackNewFrame(Frame& frame, double& time, const Vector2& newObjectPosition = Vector2(Numeric::minValue(), Numeric::minValue()), bool* lastFrameReached = nullptr, Tracking::Point::SimilarityTracker::TrackerConfidence* trackerConfidence = nullptr, Tracking::Point::SimilarityTracker::RegionTextureness* regionTextureness = nullptr);

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

		/**
		 * Move operator.
		 * @param similarityTrackerWrapper The object to be moved
		 */
		SimilarityTrackerWrapper& operator=(SimilarityTrackerWrapper&& similarityTrackerWrapper);

	protected:

		/**
		 * Not existing copy constructor.
		 * @param similarityTrackerWrapper The object to be copied (which is not allowed)
		 */
		SimilarityTrackerWrapper(SimilarityTrackerWrapper& similarityTrackerWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param similarityTrackerWrapper The object to be assigned (which is not allowed)
		 */
		SimilarityTrackerWrapper& operator=(SimilarityTrackerWrapper& similarityTrackerWrapper) = delete;

	protected:

		/// The frame medium providing the visual information for this tracker.
		Media::FrameMediumRef frameMedium_;

		/// The pixel format to be used for tracking.
		FrameType::PixelFormat trackingPixelFormat_;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// The actual implementation of the tracker.
		Tracking::Point::SimilarityTracker similarityTracker_;

		/// The for corners of the region in which the similarity tracker is working.
		Vectors2 trackingRegionCorners_;

		/// The 3DOF orientation tracker which is used to support the similarity tracker.
		Devices::OrientationTracker3DOFRef orientationTracker3DOF_;

		/// The device orientation associated with the previous frame.
		Quaternion world_Q_previousCamera_ = Quaternion(false);

		/// The camera profile of the medium.
		PinholeCamera camera_;
};

inline const Media::FrameMediumRef SimilarityTrackerWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SIMILARITYTRACKER_SIMILARITY_TRACKER_WRAPPER_H
