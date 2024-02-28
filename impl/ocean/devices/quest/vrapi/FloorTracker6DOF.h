// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_FLOOR_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_FLOOR_TRACKER_6DOF_H

#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiDevice.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements an Quest 6DOF floor tracker using VrApi.
 * @ingroup devicesquestvrapi
 */
class OCEAN_DEVICES_QUEST_VRAPI_EXPORT FloorTracker6DOF :
	virtual public VrApiDevice,
	virtual public Tracker6DOF
{
	friend class VrApiFactory;

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
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameFloorTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeFloorTracker6DOF();

	private:

		/**
		 * Creates a new floot tracker object.
		 */
		FloorTracker6DOF();

		/**
		 * Destructs an floot tracker object.
		 */
		~FloorTracker6DOF() override;

		/**
		 * Updates this controller object.
		 * @see QuestDevice::update().
		 */
		void update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp) override;

	private:

		/// The unique object id for the floor.
		ObjectId floorObjectId_ = invalidObjectId();

		/// True, if this tracker is active and delivers samples.
		bool isStarted_ = false;

		/// True, if the floor is currently tracked.
		bool floorIsTracked_ = false;
};

inline std::string FloorTracker6DOF::deviceNameFloorTracker6DOF()
{
	return std::string("Floor 6DOF Tracker");
}

inline FloorTracker6DOF::DeviceType FloorTracker6DOF::deviceTypeFloorTracker6DOF()
{
	return deviceTypeTracker6DOF();
}

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_FLOOR_TRACKER_6DOF_H
