/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Sensor.h"

namespace Ocean
{

namespace Devices
{

Sensor::Sensor(const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type)
{
	// nothing to do here
}

Sensor::~Sensor()
{
	// nothing to do here
}

std::string Sensor::translateSensorType(const SensorType sensorType)
{
	switch (sensorType)
	{
		case SENSOR_INVALID:
			return std::string("SENSOR_INVALID");

		case SENSOR_ACCELERATION_3DOF:
			return std::string("SENSOR_ACCELERATION_3DOF");

		case SENSOR_LINEAR_ACCELERATION_3DOF:
			return std::string("SENSOR_LINEAR_ACCELERATION_3DOF");

		case SENSOR_GYRO_RAW_3DOF:
			return std::string("SENSOR_GYRO_RAW_3DOF");

		case SENSOR_GYRO_UNBIASED_3DOF:
			return std::string("SENSOR_GYRO_UNBIASED_3DOF");
	}

	ocean_assert(false && "Unknown sensor type!");
	return std::string("SENSOR_INVALID");
}

Sensor::SensorType Sensor::translateSensorType(const std::string& sensorType)
{
	if (sensorType == "SENSOR_INVALID")
		return SENSOR_INVALID;

	if (sensorType == "SENSOR_ACCELERATION_3DOF")
		return SENSOR_ACCELERATION_3DOF;

	if (sensorType == "SENSOR_LINEAR_ACCELERATION_3DOF")
		return SENSOR_LINEAR_ACCELERATION_3DOF;

	if (sensorType == "SENSOR_GYRO_RAW_3DOF")
		return SENSOR_GYRO_RAW_3DOF;

	if (sensorType == "SENSOR_GYRO_UNBIASED_3DOF")
		return SENSOR_GYRO_UNBIASED_3DOF;

	ocean_assert(false && "Unknown major type!");
	return SENSOR_INVALID;
}

}

}
