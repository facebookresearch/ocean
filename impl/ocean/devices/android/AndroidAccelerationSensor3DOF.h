/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_ACCELERATION_SENSOR_3_DOF_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_ACCELERATION_SENSOR_3_DOF_H

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidSensor.h"

#include "ocean/devices/AccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements all 3DOF acceleration sensors (including the linear acceleration sensors).
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_EXPORT AndroidAccelerationSensor3DOF :
	virtual public AndroidSensor,
	virtual public AccelerationSensor3DOF
{
	friend class AndroidFactory;

	public:

		/**
		 * Returns the name of this sensor.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @return The sensor's name
		 */
		static inline std::string deviceNameAndroidAccelerationSensor3DOF(const SensorType sensorType);

		/**
		 * Returns the device type of this sensor.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @return The sensor's device type
		 */
		static inline DeviceType deviceTypeAndroidAccelerationSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new 3DOF acceleration sensor device.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @param sensor The Android sensor providing the sensor measurements, must be valid
		 */
		AndroidAccelerationSensor3DOF(const SensorType sensorType, const ASensor* sensor);

		/**
		 * Destructs a 3DOF acceleration sensor device.
		 */
		~AndroidAccelerationSensor3DOF() override;

		/**
		 * The actual event function of this device.
		 * @see AndroidDevice::onEventFunction().
		 */
		int onEventFunction() override;

	protected:

		/// The Android event timestamp of the first sensor event.
		int64_t firstAndroidEventTimestamp_ = 0ll;

		/// The Unix event timestamp of the first sensor event.
		Timestamp firstUnixEventTimestamp_ = Timestamp(false);
};

inline std::string AndroidAccelerationSensor3DOF::deviceNameAndroidAccelerationSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_ACCELERATION_3DOF || sensorType == SENSOR_LINEAR_ACCELERATION_3DOF);

	if (sensorType == SENSOR_ACCELERATION_3DOF)
	{
		return std::string("Android 3DOF Acceleration Sensor");
	}

	return std::string("Android 3DOF Linear Acceleration Sensor");
}

inline AndroidAccelerationSensor3DOF::DeviceType AndroidAccelerationSensor3DOF::deviceTypeAndroidAccelerationSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_ACCELERATION_3DOF || sensorType == SENSOR_LINEAR_ACCELERATION_3DOF);

	return deviceTypeAccelerationSensor3DOF(sensorType);
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_ACCELERATION_SENSOR_3_DOF_H
