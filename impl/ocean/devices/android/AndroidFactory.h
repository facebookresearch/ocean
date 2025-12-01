/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_FACTORY_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_FACTORY_H

#include "ocean/devices/android/Android.h"

#include "ocean/devices/Factory.h"

#include <android/sensor.h>

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements a device factory for the Android feature based tracking system.
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_ANDROID_EXPORT AndroidFactory : public Factory
{
	friend class AndroidEventDevice;

	protected:

		/**
		 * Definition of a pair combining a device type with a sensor.
		 */
		using CustomDevicePair = std::pair<Device::DeviceType, const ASensor*>;

		/**
		 * Definition of an unordered map mapping names of custom devices to sensors.
		 */
		using CustomDeviceMap = std::unordered_map<std::string, CustomDevicePair>;

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
		 * @return True, if this factory could be unregistered
		 */
		static bool unregisterFactory();

		/**
		 * Registers a custom device which is otherwise unknown to this factory.
		 * @param stringType The string type of the device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The name of the custom device, an empty string if the device could not be registered
		 */
		static std::string registerCustomDevice(const std::string& stringType, const Device::DeviceType& deviceType);

		/**
		 * Sends a new GPS location to the tracker which is managed by this factory.
		 * This function is intended for location information which is determined e.g., on the Java side.
		 * @param latitude The position's latitude, in degree, always valid, with range [-90, 90]
		 * @param longitude The position's longitude, in degree, must be valid, with range [-180, 180]
		 * @param altitude The position's altitude, in meter, NumericF::minValue() if unknown
		 * @param direction The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown
		 * @param speed The device's speed, in meter per second, with range [0, infinity), -1 if unknown
		 * @param accuracy The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown
		 * @param altitudeAccuracy The vertical accuracy in meter, with range [0, infinity), -1 if unknown
		 * @param directionAccuracy The direction accuracy, in degree, with range [0, 180], -1 if unknown
		 * @param speedAccuracy The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown
		 * @param timestamp The timestamp of the GPS location, must be valid
		 */
		static void newGPSLocation(const double latitude, const double longitude, const float altitude, const float direction, const float speed, const float accuracy, const float altitudeAccuracy, const float directionAccuracy, const float speedAccuracy, const Timestamp& timestamp);

		/**
		 * Registers a custom timestamp converter for all Android devices.
		 * @param timestampConverter The timestamp converter to be registered
		 */
		static void registerCustomTimestampConverter(TimestampConverter&& timestampConverter);

	private:

		/**
		 * Creates a new factory.
		 */
		AndroidFactory();

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
		Device* createAndroidAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF linear acceleration sensor.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		Device* createAndroidLinearAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF orientation tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		Device* createAndroidOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF raw gyro sensor.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		Device* createAndroidGyroRawSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF unbiased gyro sensor.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		Device* createAndroidGyroUnbiasedSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new GPS tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		Device* createAndroidGPSTracker(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF gravity tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		Device* createAndroidGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 3DOF heading tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new tracker, nullptr if the tracker could not be created
		 */
		Device* createAndroidHeadingTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		Device* createCustomDevice(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Registers a custom device which is otherwise unknown to this factory.
		 * @param deviceName The name of the device, must be valid
		 * @param stringType The string type of the device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return True, if the device could be registered
		 */
		bool registerCustomDevice(const std::string& deviceName, const std::string& stringType, const Device::DeviceType& deviceType);

		/**
		 * Returns the timestamp converter for all android devices.
		 * @return The timestamp converter
		 */
		static TimestampConverter& timestampConverter();

	private:

		/// The accelerometer sensor.
		const ASensor* sensorAccelerometer_ = nullptr;

		/// The linear accelerometer sensor.
		const ASensor* sensorLinearAccelerometer_ = nullptr;

		/// The rotation vector sensor.
		const ASensor* sensorRotationVector_ = nullptr;

		/// The game rotation vector sensor, this sensor does not use a magnetometer to avoid issues with changing magnetic fields near metal objects.
		const ASensor* sensorGameRotationVector_ = nullptr;

		/// The raw gyro sensor.
		const ASensor* sensorGyroRaw_ = nullptr;

		/// The unbiased gyro sensor.
		const ASensor* sensorGyroUnbiased_ = nullptr;

		/// The gravity sensor.
		const ASensor* sensorGravity_ = nullptr;

		/// The custom devices.
		CustomDeviceMap customDeviceMap_;

		/// The factory's lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_FACTORY_H
