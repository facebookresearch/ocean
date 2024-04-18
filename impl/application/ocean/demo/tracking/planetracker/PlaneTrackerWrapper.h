// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_PLANE_TRACKER_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_PLANE_TRACKER_WRAPPER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "metaonly/ocean/tracking/plane/PlaneTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingplanetracker Plane Tracker
 * @{
 * The demo application shows the capabilities of Ocean's plane tracker.<br>
 * The actual implementation is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent wrapper for the plane tracker which will be used/shared by/across platform specific applications.
 * Beware: You must not have more than one instance within one application.
 * @ingroup applicationdemotrackingplanetracker
 */
class PlaneTrackerWrapper
{
	public:

		/**
		 * Creates an invalid tracker object.
		 */
		PlaneTrackerWrapper();

		/**
		 * Move constructor.
		 * @param planeTrackerWrapper The tracker object to be moved
		 */
		PlaneTrackerWrapper(PlaneTrackerWrapper&& planeTrackerWrapper) noexcept;

		/**
		 * Creates a new tracker object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of the tracker
		 */
		explicit PlaneTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an tracker object.
		 */
		~PlaneTrackerWrapper();

		/**
		 * Explicitly releases this tracker object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking update for the frame.
		 * @param frame The resulting frame showing the blending between the current video frame and the previous frame, may be invalid if the tracker fails
		 * @param time The time the tracker needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param newPlanePosition The location of the new plane within the resolution of the frame, otherwise an invalid position
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True, if a new frame was available
		 */
		bool trackNewFrame(Frame& frame, double& time, const Vector2& newPlanePosition = Vector2(Numeric::minValue(), Numeric::minValue()), bool* lastFrameReached = nullptr);

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

		/**
		 * Move operator.
		 * @param planeTrackerWrapper The object to be moved
		 */
		PlaneTrackerWrapper& operator=(PlaneTrackerWrapper&& planeTrackerWrapper) noexcept;

	protected:

		/**
		 * Not existing copy constructor.
		 * @param planeTrackerWrapper The object to be copied (which is not allowed)
		 */
		PlaneTrackerWrapper(PlaneTrackerWrapper& planeTrackerWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param planeTrackerWrapper The object to be assigned (which is not allowed)
		 */
		PlaneTrackerWrapper& operator=(PlaneTrackerWrapper& planeTrackerWrapper) = delete;

	protected:

		/// The frame medium providing the visual information for this tracker.
		Media::FrameMediumRef frameMedium_;

		/// The camera profile of the input frame medium.
		PinholeCamera camera_;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// The actual implementation of the PlaneTracker.
		Tracking::Plane::PlaneTracker planeTracker_;

		/// The 3DOF orientation tracker which is used to support the PlaneTracker.
		Devices::OrientationTracker3DOFRef orientationTracker3DOF_;
};

inline const Media::FrameMediumRef PlaneTrackerWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_PLANE_TRACKER_WRAPPER_H
