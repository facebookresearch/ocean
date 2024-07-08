/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_POINT_TRACKER_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_POINT_TRACKER_WRAPPER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

 #include "ocean/tracking/point/PointTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingpointtracker Point Tracker
 * @{
 * The demo application demonstrates the usage of the PointTracker able to track points from one frame to another while keeping the history from previous frames.<br>
 * The back facing camera of the iOS device is used as tracking input source.<br>
 * This demo application uses the platform independent PointTracker class implementing most of the necessary code.
 * However, this application here is intended for Apple iOS platforms only.
 * The implementation of the basic feature tracker functionality is platform independent, thus is can be used on any platform.
 *
 * The following wiki page provides more details about the demo application:<br>
 * https://facebookresearch.github.io/ocean/docs/demoapps/crossplatformapps/point_tracker/
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent point tracker functionality which will be used/shared by/across platform specific applications.
 * @ingroup applicationdemotrackingpointtracker
 */
class PointTrackerWrapper
{
	public:

		/**
		 * Creates an invalid point tracker object.
		 */
		PointTrackerWrapper();

		/**
		 * Creates a new point tracker object by a given set of command arguments.
		 * The command arguments can be used to specify the behavior/mode of the tracker:<br>
		 *
		 * 1. Parameter (optional): The name or filename of the input source e.g.:
		 * "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png"
		 *
		 * 2. Parameter (optional): The preferred frame dimension of the input medium in pixel:
		 * "640x480", or "1280x720", or "1920x1080"
		 *
		 * @param commandArguments The command arguments that configure the properties of the point tracker
		 */
		explicit PointTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a point tracker object.
		 */
		~PointTrackerWrapper();

		/**
		 * Explicitly releases this point tracker object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking for the frame.
		 * @param frame The resulting frame showing a virtual box (with coordinate system) around the pattern (if detected)
		 * @param time The time the tracker needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity), negative if the pattern has not been detected yet
		 * @return True, if succeeded
		 */
		bool trackNewFrame(Frame& frame, double& time);

		/**
		 * Uses the next tracking mode of the tracker.
		 * In case the last tracking mode is reached, the first tracking mode is used.
		 */
		void nextTrackingMode();

		/**
		 * Returns the current tracking mode as readable string.
		 * @return The tracker's current tracking mode
		 */
		std::string trackingMode() const;

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

	protected:

		/// The frame medium providing the visual information for this point tracker.
		Media::FrameMediumRef frameMedium_;

		/// The actual tracker to be used for point tracking.
		Tracking::Point::PointTracker pointTracker_;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

inline const Media::FrameMediumRef PointTrackerWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_POINT_TRACKER_WRAPPER_H
