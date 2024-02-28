// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_CONTROLLER_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_CONTROLLER_TRACKER_6DOF_H

#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiDevice.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements an Quest 6DOF controller tracker using VrApi.
 * @ingroup devicesquestvrapi
 */
class OCEAN_DEVICES_QUEST_VRAPI_EXPORT ControllerTracker6DOF :
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
		static inline std::string deviceNameControllerTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeControllerTracker6DOF();

	private:

		/**
		 * Creates a new controller tracker object.
		 */
		ControllerTracker6DOF();

		/**
		 * Destructs an controller tracker object.
		 */
		~ControllerTracker6DOF() override;

		/**
		 * Updates this controller object.
		 * @see QuestDevice::update().
		 */
		void update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp) override;

	private:

		/// The unique object id for the left controller.
		ObjectId objectIdLeft_ = invalidObjectId();

		/// The unique object id for the right controller.
		ObjectId objectIdRight_ = invalidObjectId();

		/// True, if this tracker is active and delivers samples.
		bool isStarted_ = false;

		/// The object ids of all currently tracked controllers (at most two).
		ObjectIdSet trackedIds_;
};

inline std::string ControllerTracker6DOF::deviceNameControllerTracker6DOF()
{
	return std::string("Quest Controller 6DOF Tracker");
}

inline ControllerTracker6DOF::DeviceType ControllerTracker6DOF::deviceTypeControllerTracker6DOF()
{
	return deviceTypeTracker6DOF();
}

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_CONTROLLER_TRACKER_6DOF_H
