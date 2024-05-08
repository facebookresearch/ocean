/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_GYRO_SENSOR_3_DOF_H
#define META_OCEAN_DEVICES_IOS_IOS_GYRO_SENSOR_3_DOF_H

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSSensor.h"
#include "ocean/devices/ios/MotionManager.h"

#include "ocean/devices/GyroSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements all 3DOF gyro sensors (including the raw and unbiased gyro sensors).
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSGyroSensor3DOF :
	virtual public IOSSensor,
	virtual public GyroSensor3DOF
{
	friend class IOSFactory;

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		virtual bool start();

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		virtual bool pause();

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		virtual bool stop();

		/**
		 * Returns the name of this sensor.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_GYRO_RAW_3DOF or SENSOR_GYRO_UNBIASED_3DOF
		 * @return The sensor's name
		 */
		static inline std::string deviceNameIOSGyroSensor3DOF(const SensorType sensorType);

		/**
		 * Returns the device type of this sensor.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_GYRO_RAW_3DOF or SENSOR_GYRO_UNBIASED_3DOF
		 * @return The sensor's device type
		 */
		static inline DeviceType deviceTypeIOSGyroSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new 3DOF gyro sensor device.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_GYRO_RAW_3DOF or SENSOR_GYRO_UNBIASED_3DOF
		 */
		IOSGyroSensor3DOF(const SensorType sensorType);

		/**
		 * Destructs a 3DOF gyro sensor device.
		 */
		virtual ~IOSGyroSensor3DOF();

	private:

		/**
		 * Callback function when using SENSOR_GYRO_UNBIASED_3DOF.
		 * @param deviceMotion iOS DeviceMotion readout
		 */
		void onDeviceMotion(CMDeviceMotion* deviceMotion);

	private:

		// Callback id when using SENSOR_GYRO_UNBIASED_3DOF.
		MotionManager::ListenerId deviceMotionListenerId_ = MotionManager::invalidListenerId();
};

inline std::string IOSGyroSensor3DOF::deviceNameIOSGyroSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_GYRO_RAW_3DOF || sensorType == SENSOR_GYRO_UNBIASED_3DOF);

	if (sensorType == SENSOR_GYRO_RAW_3DOF)
	{
		return std::string("IOS 3DOF Gyro Raw Sensor");
	}

	return std::string("IOS 3DOF Gyro Unbiased Sensor");
}

inline IOSGyroSensor3DOF::DeviceType IOSGyroSensor3DOF::deviceTypeIOSGyroSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_GYRO_RAW_3DOF || sensorType == SENSOR_GYRO_UNBIASED_3DOF);

	return deviceTypeGyroSensor3DOF(sensorType);
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_GYRO_SENSOR_3_DOF_H
