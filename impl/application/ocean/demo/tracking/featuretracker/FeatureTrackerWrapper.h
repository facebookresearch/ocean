/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_FEATURE_TRACKER_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_FEATURE_TRACKER_WRAPPER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/DevicePlayer.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/tracking/VisualTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingfeaturetracker Feature Tracker
 * @{
 * The demo application demonstrates the usage of the "Pattern 6DOF Tracker" or "ORB Feature Based 6DOF Tracker" tracker.
 * The user has the possibility to define the video input source (with preferred frame dimension), the pattern, and the used tracker via the commando line parameter.<br>
 * The application will visualize a bounding box and a coordinate system in every input frame for which a valid camera pose could be determined.<br>
 * The implementation of the basic feature tracker functionality is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent feature tracker functionality which will be used/shared by/across platform specific applications.
 * @ingroup applicationdemotrackingfeaturetracker
 */
class FeatureTrackerWrapper
{
	public:

		/**
		 * Creates an invalid feature tracker object.
		 */
		FeatureTrackerWrapper();

		/**
		 * Move constructor.
		 * @param featureTrackerWrapper The feature tracker object to be moved
		 */
		inline FeatureTrackerWrapper(FeatureTrackerWrapper&& featureTrackerWrapper) noexcept;

		/**
		 * Creates a new feature tracker object by a given set of command arguments.
		 * For a full list of the command arguments that can be used to specify the behavior/mode of the tracker, run with `--help`.
		 * @param commandArguments The command arguments that configure the properties of the feature tracker
		 */
		explicit FeatureTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a feature tracker wrapper object.
		 */
		~FeatureTrackerWrapper();

		/**
		 * Explicitly releases this feature tracker object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking for the frame.
		 * @param resultFrame The resulting frame showing a virtual box (with coordinate system) around the pattern (if detected)
		 * @param time The time the tracker needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity), negative if the pattern has not been detected yet
		 * @return True, if succeeded
		 */
		bool trackNewFrame(Frame& resultFrame, double& time);

		/**
		 * Returns the input medium which is used.
		 * @return The tracker's input medium
		 */
		inline Media::FrameMediumRef inputMedium() const;

		/**
		 * Move operator.
		 * @param featureTracker The tracker object to be moved
		 * @return Reference to this object
		 */
		inline FeatureTrackerWrapper& operator=(FeatureTrackerWrapper&& featureTracker) noexcept;

	protected:

		/// The frame medium providing the visual input information for this feature tracker.
		Media::FrameMediumRef inputMedium_;

		/// The bounding box  of the tracking pattern defined in the world coordinate system, with range (0, infinity)x(0, infinity)
		Box3 objectDimension_;

		/// The actual tracker to be used for feature tracking.
		Tracking::VisualTrackerRef visualTracker_;

		/// The camera profile defining the project and the camera distortion.
		SharedAnyCamera anyCamera_;

		/// The timestamp of the last frame that has been handled.
		Timestamp lastHandledFrameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// The 3DOF orientation tracker which is used to support the tracker.
		Devices::OrientationTracker3DOFRef orientationTracker3DOF_;

		/// The device player which may be used to replay a recording.
		Devices::SharedDevicePlayer devicePlayer_;
};

inline FeatureTrackerWrapper::FeatureTrackerWrapper(FeatureTrackerWrapper&& featureTrackerWrapper) noexcept
{
	*this = std::move(featureTrackerWrapper);
}

inline Media::FrameMediumRef FeatureTrackerWrapper::inputMedium() const
{
	return inputMedium_;
}

inline FeatureTrackerWrapper& FeatureTrackerWrapper::operator=(FeatureTrackerWrapper&& featureTrackerWrapper) noexcept
{
	if (this != &featureTrackerWrapper)
	{
		devicePlayer_ = std::move(featureTrackerWrapper.devicePlayer_);

		inputMedium_ = std::move(featureTrackerWrapper.inputMedium_);
		objectDimension_ = std::move(featureTrackerWrapper.objectDimension_);

		visualTracker_ = std::move(featureTrackerWrapper.visualTracker_);
		anyCamera_ = std::move(featureTrackerWrapper.anyCamera_);

		lastHandledFrameTimestamp_ = featureTrackerWrapper.lastHandledFrameTimestamp_;
		performance_ = featureTrackerWrapper.performance_;

		orientationTracker3DOF_ = std::move(featureTrackerWrapper.orientationTracker3DOF_);
	}

	return *this;
}

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER

/**
 * Creates a device player.
 * @param commandArguments The command arguments to use
 * @return The resulting device player, nullptr if the player could not be created
 */
Devices::SharedDevicePlayer FeatureTrackerWrapper_createExternalDevicePlayer(const CommandArguments& commandArguments);

#endif // OCEAN_USE_EXTERNAL_DEVICE_PLAYER

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_FEATURE_TRACKER_WRAPPER_H
