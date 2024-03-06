// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#ifdef OCEAN_USE_DEVICES_VRS
	#include "ocean/devices/vrs/DevicePlayer.h"
#endif

#include "ocean/io/File.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/SampleMap.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/tracking/oculustags/OculusTagTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingoculustagsoculustagtracker Oculus Tag Tracker
 * @{
 * The demo application shows the capabilities of Ocean's Oculus Tag tracker.<br>
 * The actual implementation is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent wrapper for the plane tracker which will be used/shared by/across platform specific applications.
 * Beware: You must not have more than one instance within one application.
 * @ingroup applicationdemotrackingoculustagsoculustagtracker
 */
class OculusTagTrackerWrapper
{
	protected:

		/**
		 * Definition of a sample map for 6-DOF poses a a device.
		 */
		typedef SampleMap<HomogenousMatrixD4> DevicePoses;

	public:

		/**
		 * Creates an invalid tracker object.
		 */
		OculusTagTrackerWrapper();

		/**
		 * Move constructor.
		 * @param oculusTagTrackerWrapper The tracker object to be moved
		 */
		OculusTagTrackerWrapper(OculusTagTrackerWrapper&& oculusTagTrackerWrapper) noexcept;

		/**
		 * Creates a new tracker object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of the tracker
		 */
		explicit OculusTagTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an tracker object.
		 */
		~OculusTagTrackerWrapper();

		/**
		 * Explicitly releases this tracker object.
		 */
		void release();

		/**
		 * Applies the next tracking iteration for the next frame.
		 * @param resultFrame The resulting frame showing the tracking result, may be invalid if the tracker fails
		 * @param time The time the tracker needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @return True, if a new frame was available
		 */
		bool trackNewFrame(Frame& resultFrame, double& time);

		/**
		 * Move operator.
		 * @param oculusTagTrackerWrapper The object to be moved
		 */
		OculusTagTrackerWrapper& operator=(OculusTagTrackerWrapper&& oculusTagTrackerWrapper) noexcept;

	protected:

		/**
		 * Deleted copy constructor.
		 */
		OculusTagTrackerWrapper(OculusTagTrackerWrapper&) = delete;

		/**
		 * Deleted assign operator.
		 */
		OculusTagTrackerWrapper& operator=(OculusTagTrackerWrapper&) = delete;

	protected:

		/// The frame media that provide the camera frames
		Media::FrameMediumRefs frameMediumRefs_;

		/// The time stamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// The Oculus Tag tracker object.
		Tracking::OculusTags::OculusTagTracker oculusTagTracker_;

		/// The tag size information that is loaded from a VRS file
		std::map<double, Tracking::OculusTags::TagSizeMap> map_vrsTagSizeMap_;

		/// The default tag size that is loaded from a VRS file
		Scalar vrsDefaultTagSize_;

		/// The tracked tags that are loaded from a VRS file
		std::map<double, Tracking::OculusTags::OculusTags> map_vrsTrackedTags_;

		/// The image counter.
		unsigned int frameCounter_ = 0u;

		/// Indicates how to replay the input, at normal speed or stop-motion
		bool enableStopMotionReplay_ = true;

#ifdef OCEAN_USE_DEVICES_VRS

		/// Device player that is used for VRS replay
		std::shared_ptr<Devices::VRS::DevicePlayer> devicePlayer_;

#endif

		/// The device poses that will be extracted from the VRS file
		SampleMap<HomogenousMatrixD4> world_T_devices_;

		/// A movie recorder to create visualizations of the results
		Media::MovieRecorderRef movieRecorder_;
};
