/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_ACCELERATION_SENSOR_3_DOF_H
#define META_OCEAN_DEVICES_IOS_IOS_ACCELERATION_SENSOR_3_DOF_H

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSSensor.h"
#include "ocean/devices/ios/MotionManager.h"

#include "ocean/devices/AccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements all 3DOF acceleration sensors (including the linear acceleration sensors).
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSAccelerationSensor3DOF :
	virtual public IOSSensor,
	virtual public AccelerationSensor3DOF
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
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @return The sensor's name
		 */
		static inline std::string deviceNameIOSAccelerationSensor3DOF(const SensorType sensorType);

		/**
		 * Returns the device type of this sensor.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 * @return The sensor's device type
		 */
		static inline DeviceType deviceTypeIOSAccelerationSensor3DOF(const SensorType sensorType);

	protected:

		/**
		 * Creates a new 3DOF acceleration sensor device.
		 * @param sensorType The type of the sensor (the subtype of the device), can be SENSOR_ACCELERATION_3DOF or SENSOR_LINEAR_ACCELERATION_3DOF
		 */
		IOSAccelerationSensor3DOF(const SensorType sensorType);

		/**
		 * Destructs a 3DOF acceleration sensor device.
		 */
		virtual ~IOSAccelerationSensor3DOF();

	private:

		/**
		 * Callback function when using SENSOR_LINEAR_ACCELERATION_3DOF.
		 * @param deviceMotion iOS DeviceMotion readout
		 */
		void onDeviceMotion(CMDeviceMotion* deviceMotion);

	private:

		// Callback id when using SENSOR_LINEAR_ACCELERATION_3DOF.
		MotionManager::ListenerId deviceMotionListenerId_ = MotionManager::invalidListenerId();
};

inline std::string IOSAccelerationSensor3DOF::deviceNameIOSAccelerationSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_ACCELERATION_3DOF || sensorType == SENSOR_LINEAR_ACCELERATION_3DOF);

	if (sensorType == SENSOR_ACCELERATION_3DOF)
	{
		return std::string("IOS 3DOF Acceleration Sensor");
	}

	return std::string("IOS 3DOF Linear Acceleration Sensor");
}

inline IOSAccelerationSensor3DOF::DeviceType IOSAccelerationSensor3DOF::deviceTypeIOSAccelerationSensor3DOF(const SensorType sensorType)
{
	ocean_assert(sensorType == SENSOR_ACCELERATION_3DOF || sensorType == SENSOR_LINEAR_ACCELERATION_3DOF);

	return deviceTypeAccelerationSensor3DOF(sensorType);
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_ACCELERATION_SENSOR_3_DOF_H
