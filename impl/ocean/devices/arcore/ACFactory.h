/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_FACTORY_H
#define META_OCEAN_DEVICES_ARCORE_AC_FACTORY_H

#include "ocean/devices/arcore/ARCore.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/**
 * This class implements a device factory for the ARCore tracking library.
 * @ingroup devicesarcore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACFactory : public Factory
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
		 * Updates the factory's trackers with the most recent tracking results from ARCore.
		 * This function must be called out of the main GL rendering thread.
		 * @param textureId The id of the texture in which the camera image will be copied, must be valid
		 */
		static void update(unsigned int textureId);

	private:

		/**
		 * Creates a new factory.
		 */
		ACFactory();

		/**
		 * Registers all devices.
		 */
		void registerDevices();

		/**
		 * Creates a new 6DOF tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createACTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_FACTORY_H
