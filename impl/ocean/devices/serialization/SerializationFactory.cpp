/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationFactory.h"
#include "ocean/devices/serialization/SerializationAccelerationSensor3DOF.h"
#include "ocean/devices/serialization/SerializationGPSTracker.h"
#include "ocean/devices/serialization/SerializationGravityTracker3DOF.h"
#include "ocean/devices/serialization/SerializationGyroSensor3DOF.h"
#include "ocean/devices/serialization/SerializationOrientationTracker3DOF.h"
#include "ocean/devices/serialization/SerializationPositionTracker3DOF.h"
#include "ocean/devices/serialization/SerializationTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

std::string nameSerializationLibrary();

SerializationFactory::SerializationFactory() :
	Factory(nameSerializationLibrary())
{
	// nothing to do here
}

bool SerializationFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new SerializationFactory()));
}

bool SerializationFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameSerializationLibrary());
}

Device* SerializationFactory::createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationTracker6DOF::deviceTypeSerializationTracker6DOF(), deviceType);

	return new SerializationTracker6DOF(name);
}

Device* SerializationFactory::createGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationGPSTracker::deviceTypeSerializationGPSTracker(), deviceType);

	return new SerializationGPSTracker(name);
}

Device* SerializationFactory::createAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	const Sensor::SensorType sensorType = Sensor::SensorType(deviceType.minorType());

	ocean_assert(sensorType == Sensor::SENSOR_ACCELERATION_3DOF || sensorType == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF);

	if (sensorType == Sensor::SENSOR_ACCELERATION_3DOF || sensorType == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF)
	{
		return new SerializationAccelerationSensor3DOF(name, sensorType);
	}

	ocean_assert(false && "Device type is not supported!");
	return nullptr;
}

Device* SerializationFactory::createGyroSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	const Sensor::SensorType sensorType = Sensor::SensorType(deviceType.minorType());

	ocean_assert(sensorType == Sensor::SENSOR_GYRO_RAW_3DOF || sensorType == Sensor::SENSOR_GYRO_UNBIASED_3DOF);

	if (sensorType == Sensor::SENSOR_GYRO_RAW_3DOF || sensorType == Sensor::SENSOR_GYRO_UNBIASED_3DOF)
	{
		return new SerializationGyroSensor3DOF(name, sensorType);
	}

	ocean_assert(false && "Device type is not supported!");
	return nullptr;
}

Device* SerializationFactory::createOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationOrientationTracker3DOF::deviceTypeSerializationOrientationTracker3DOF(), deviceType);

	return new SerializationOrientationTracker3DOF(name);
}

Device* SerializationFactory::createGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationGravityTracker3DOF::deviceTypeSerializationGravityTracker3DOF(), deviceType);

	return new SerializationGravityTracker3DOF(name);
}

Device* SerializationFactory::createPositionTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationPositionTracker3DOF::deviceTypeSerializationPositionTracker3DOF(), deviceType);

	return new SerializationPositionTracker3DOF(name);
}

}

}

}
