/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_FACTORY_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_FACTORY_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

/**
 * This class implements a device factory for the Serialization library.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationFactory : public Factory
{
	friend class SerializerDevicePlayer;

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
		SerializationFactory();

		/**
		 * Creates a new 6-DOF Serialization tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new GPS Serialization tracker.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createGPSTracker(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF acceleration Serialization sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new sensor, must be valid
		 * @param deviceType The device type of the sensor, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF gyro Serialization sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new sensor, must be valid
		 * @param deviceType The device type of the sensor, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createGyroSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF orientation Serialization tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF gravity Serialization tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF position Serialization tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		static Device* createPositionTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_FACTORY_H
