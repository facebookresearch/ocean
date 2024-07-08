/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_FACE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_FACE_TRACKER_6_DOF_H

#include "ocean/devices/arkit/ARKit.h"
#include "ocean/devices/arkit/AKDevice.h"

#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements the 6DOF face tracker providing only the head pose.
 * ARKit's face tracker can combine SLAM/World tracker. Face tracking is always executed on the user-facing camera while the back-facing camera is used for SLAM/World tracking.<br>
 * Therefore, the input medium should always be the back-facing camera instead of the user-facing camera.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKFaceTracker6DOF :
	virtual public AKDevice,
	virtual public Tracker6DOF,
	virtual public VisualTracker
{
	friend class AKFactory;

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param timestamp The timestamp of the new transformation
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(13.0))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame);

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKFaceTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKFaceTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF face tracker.
		 */
		explicit AKFaceTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~AKFaceTracker6DOF() override;

	protected:

		/// The unique id for the face object.
		ObjectId faceObjectId_ = invalidObjectId();

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// True, if the face object is currently tracked.
		bool faceIsTracked_ = false;
};

inline std::string AKFaceTracker6DOF::deviceNameAKFaceTracker6DOF()
{
	return std::string("ARKit 6DOF Face Tracker");
}

inline AKFaceTracker6DOF::DeviceType AKFaceTracker6DOF::deviceTypeAKFaceTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_FACE_TRACKER_6_DOF_H
