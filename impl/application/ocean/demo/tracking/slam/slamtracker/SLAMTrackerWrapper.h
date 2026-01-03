/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_SLAM_TRACKER_WRAPPER_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_SLAM_TRACKER_WRAPPER_H

#include "application/ocean/demo/tracking/slam/ApplicationDemoTrackingSLAM.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/tracking/slam/TrackerMono.h"

#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/devices/serialization/SerializerDeviceRecorder.h"

/**
 * @ingroup applicationdemotrackingslam
 * @defgroup applicationdemotrackingslamslamtracker SLAM Tracker
 * @{
 * The demo application shows the capabilities of Ocean's SLAM Tracker.<br>
 * The implementation of the basic image aligner functionality is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent wrapper for the SLAM tracker which will be used/shared by/across platform specific applications.
 * Beware: You must not have more than one instance within one application.
 * @ingroup applicationdemotrackingslamslamtracker
 */
class SLAMTrackerWrapper
{
	protected:

		/**
		 * Helper class providing access to gravity and orientation sensor data.
		 * This class manages GravityTracker3DOF and OrientationTracker3DOF devices and provides convenience functions to retrieve sensor measurements transformed into the camera coordinate system.
		 * @ingroup applicationdemotrackingslamreplay
		 */
		class SensorAccessor
		{
			public:

				/**
				 * Creates a new sensor accessor object.
				 */
				SensorAccessor() = default;

				/**
				 * Returns the gravity vector in the camera coordinate system.
				 * On first call, this function will initialize and start the gravity tracker device.
				 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
				 * @param timestamp The timestamp for which the gravity measurement is requested
				 * @return The gravity vector in camera coordinates, a null vector if unavailable
				 */
				Vector3 cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp);

				/**
				 * Returns the camera's orientation in world coordinates.
				 * On first call, this function will initialize and start the orientation tracker device.
				 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
				 * @param timestamp The timestamp for which the orientation measurement is requested
				 * @return The rotation transforming points in camera coordinates to world coordinates, invalid if unavailable
				 */
				Quaternion anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp);

				/**
				 * Releases all device references held by this object.
				 */
				void release();

			protected:

				/// The gravity tracker device providing gravity measurements.
				Devices::GravityTracker3DOFRef gravityTracker_;

				/// The orientation tracker device providing orientation measurements.
				Devices::OrientationTracker3DOFRef orientationTracker_;
		};

	public:

		/**
		 * Creates an invalid aligner object.
		 */
		SLAMTrackerWrapper() = default;

		/**
		 * Move constructor.
		 * @param slamTrackerWrapper The tracker object to be moved
		 */
		SLAMTrackerWrapper(SLAMTrackerWrapper&& slamTrackerWrapper) noexcept;

		/**
		 * Creates a new aligner object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of the aligner
		 */
		explicit SLAMTrackerWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a tracker object.
		 */
		~SLAMTrackerWrapper();

		/**
		 * Explicitly releases this aligner object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking update for the frame.
		 * @param outputFrame The resulting frame showing the blending between the current video frame and the previous frame, may be invalid if the tracker fails
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True, if a new frame was available
		 */
		bool trackNewFrame(Frame& outputFrame, bool* lastFrameReached = nullptr);

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

		/**
		 * Starts a recording.
		 * @return True, if succeeded
		 */
		bool startRecording();

		/**
		 * Stops a recording.
		 * @return True, if succeeded
		 */
		bool stopRecording();

		/**
		 * Move operator.
		 * @param slamTrackerWrapper The object to be moved
		 */
		SLAMTrackerWrapper& operator=(SLAMTrackerWrapper&& slamTrackerWrapper) noexcept;

	protected:

		/**
		 * Not existing copy constructor.
		 */
		SLAMTrackerWrapper(SLAMTrackerWrapper&) = delete;

		/**
		 * Not existing assign operator.
		 */
		SLAMTrackerWrapper& operator=(SLAMTrackerWrapper&) = delete;

	protected:

		/// The frame medium providing the visual information for this tracker.
		Media::FrameMediumRef frameMedium_;

		/// The recent camera frame, with pixel format FORMAT_Y8.
		Frame yFrame_;

		/// The camera clipper of the camera model.
		AnyCameraClipper cameraClipper_;

		/// True to downsample the input image by factor 2; False otherwise.
		bool downsample_ = false;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// The accessor object providing access to gravity and orientation sensor data.
		SensorAccessor sensorAccessor_;

		/// The actual implementation of the tracker.
		Tracking::SLAM::TrackerMono slamTracker_;

		/// The timestamp of the last warmup frame.
		Timestamp warmupTimestamp_;

		/// The device recorder which will be used to record the tracking results.
		std::unique_ptr<Devices::Serialization::SerializerDeviceRecorder> deviceRecorder_;

		/// The lock for the device recorder.
		Lock deviceRecorderLock_;

};

inline const Media::FrameMediumRef SLAMTrackerWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_SLAM_TRACKER_WRAPPER_H
