/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationGyroSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationGyroSensor3DOF::SerializationGyroSensor3DOF(const std::string& name, const SensorType sensorType) :
	Device(name, deviceTypeSerializationGyroSensor3DOF(sensorType)),
	Measurement(name, deviceTypeSerializationGyroSensor3DOF(sensorType)),
	Sensor(name, deviceTypeSerializationGyroSensor3DOF(sensorType)),
	GyroSensor3DOF(name, sensorType),
	SerializationSensor(name, deviceTypeSerializationGyroSensor3DOF(sensorType))
{
	// nothing to do here
}

SerializationGyroSensor3DOF::~SerializationGyroSensor3DOF()
{
	// nothing to do here
}

void SerializationGyroSensor3DOF::forwardSampleEvent(Gyro3DOFSample::Measurements&& measurements, const Timestamp& timestamp, Metadata&& /*metadata*/)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ObjectIds objectIds;
	objectIds.reserve(measurements.size());

	for (size_t n = 0; n < measurements.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new Gyro3DOFSample(timestamp, std::move(objectIds), std::move(measurements))));
}

}

}

}
