// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_DEVICE_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_DEVICE_H

#include "ocean/devices/quest/vrapi/VrApi.h"

#include "ocean/devices/Device.h"

#include "ocean/platform/meta/quest/Device.h"

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
 * This class implements a base class for all devices based on VrApi.
 * @ingroup devicesquestvrapi
 */
class OCEAN_DEVICES_QUEST_VRAPI_EXPORT VrApiDevice : virtual public Device
{
	friend class VrApiFactory;

	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

	protected:

		/**
		 * Creates a new device by is name.
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		VrApiDevice(const std::string& name, const DeviceType type);

		/**
		 * Updates this controller object.
		 * This function should be called once per frame.
		 * @param ovr The ovr mobile object, must be valid
		 * @param deviceType The type of the Quest device, must be valid
		 * @param trackedRemoteDevice The tracked remote device allowing access to the controllers
		 * @param timestamp The timestamp of the current frame, must be valid
		 */
		virtual void update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp) = 0;
};

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_DEVICE_H
