// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_FACTORY_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_FACTORY_H

#include "ocean/devices/quest/vrapi/VrApi.h"

#include "ocean/devices/Factory.h"

#include "metaonly/ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements a device factory for the Quest devices using VrApi.
 * @ingroup devicesquestvrapi
 */
class OCEAN_DEVICES_QUEST_VRAPI_EXPORT VrApiFactory : public Factory
{
	public:

		/**
		 * Registers this factory at the manager.
		 * Beware: Unregister this factory if not needed anymore.
		 * @return True, if this factory hasn't been registered before
		 */
		static bool registerFactory();

		/**
		 * Unregisters this factory at the manger.
		 * This unregistration should be done after all created devices have been released.
		 * @return True, if this factory chould be unregistered
		 */
		static bool unregisterFactory();

		/**
		 * Updates all elements of the Quest library.
		 * This function should be called once per frame.
		 * @param ovr The ovr mobile object, must be valid
		 * @param deviceType The type of the Quest device, must be valid
		 * @param trackedRemoteDevice The tracked remote device allowing access to the controllers
		 * @param timestamp The timestamp of the current frame, must be valid
		 */
		static void update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp);

	private:

		/**
		 * Creates a new factory.
		 */
		VrApiFactory();

		/**
		 * Creates the 6DOF tracker for the controllers.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 * @return The new 6DOF tracker
		 */
		static Device* createControllerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates the 6DOF tracker for the floor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 * @return The new 6DOF tracker
		 */
		static Device* createFloorTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates the 6DOF tracker for the headset.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 * @return The new 6DOF tracker
		 */
		static Device* createHeadsetTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates the 6DOF on device relocalizer tracker for Quest devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 * @return The new 6DOF tracker
		 */
		static Device* createOnDeviceRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_VR_API_FACTORY_H
