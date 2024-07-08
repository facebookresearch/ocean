/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_FACTORY_H
#define META_OCEAN_DEVICES_IOS_IOS_FACTORY_H

#include "ocean/devices/ios/IOS.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a device factory for the IOS feature based tracking system.
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSFactory : public Factory
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

	private:

		/**
		 * Creates a new factory.
		 */
		IOSFactory();

		/**
		 * Registers all devices.
		 */
		void registerDevices();

		/**
		 * Creates a new 3DOF acceleration sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createIOSAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF linear acceleration sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createIOSLinearAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF orientation tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the sensor could not be created
		 */
		static Device* createIOSOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF raw gyro sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createIOSGyroRawSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF unbiased gyro sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createIOSGyroUnbiasedSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new GPS tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createIOSGPSTracker(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new Gravity tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createIOSGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new Heading tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createIOSHeadingTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_FACTORY_H
