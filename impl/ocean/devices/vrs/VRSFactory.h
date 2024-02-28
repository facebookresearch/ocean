// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_FACTORY_H
#define META_OCEAN_DEVICES_VRS_VRS_FACTORY_H

#include "ocean/devices/vrs/VRS.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * This class implements a device factory for the VRS library.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSFactory : public Factory
{
	friend class DevicePlayer;

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

	protected:

		/**
		 * Creates a new factory.
		 */
		VRSFactory();

		/**
		 * Creates a new 6-DOF VRS tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new GPS VRS tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createGPSTracker(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_FACTORY_H
