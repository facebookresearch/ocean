/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H

#include "ocean/devices/slam/SLAM.h"
#include "ocean/devices/slam/SLAMDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/tracking/slam/TrackerMono.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

/**
 * This class implements an SLAM feature based tracker.
 * @ingroup devicesslam
 */
class OCEAN_DEVICES_SLAM_EXPORT SLAMTracker6DOF :
	virtual public SLAMDevice,
	virtual public Tracker6DOF,
	virtual public VisualTracker,
	protected Thread
{
	friend class SLAMFactory;

	public:

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Sets an abstract parameter of this device.
		 * @see Device::setParameter().
		 */
		bool setParameter(const std::string& parameter, const Value& value) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameSLAMTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSLAMTracker6DOF();

	private:

		/**
		 * Creates a new SLAM feature based 6DOF tracker object.
		 */
		SLAMTracker6DOF();

		/**
		 * Destructs an SLAM feature based 6DOF tracker object.
		 */
		~SLAMTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Posts a new camera pose.
		 * @param world_T_camera The camera pose to post, must be valid
		 * @param timestamp The timestamp of the frame to which the pose belongs
		 */
		void postPose(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp);

	protected:

		/// The actual implementation of the tracker.
		Tracking::SLAM::TrackerMono trackerMono_;

		/// The unique object id of this tracker.
		ObjectId uniqueObjectId_ = invalidObjectId();

		/// True, if the SLAM tracker is currently actively providing a valid camera pose.
		std::atomic_bool isObjectTracked_ = false;

		/// The preferred width of the camera frame.
		unsigned int preferredCameraWidth_ = 0u;

		/// The preferred height of the camera frame.
		unsigned int preferredCameraHeight_ = 0u;
};

inline std::string SLAMTracker6DOF::deviceNameSLAMTracker6DOF()
{
	return std::string("SLAM Feature Based 6DOF Tracker");
}

inline SLAMTracker6DOF::DeviceType SLAMTracker6DOF::deviceTypeSLAMTracker6DOF()
{
	return SLAMTracker6DOF::DeviceType(SLAMTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H
