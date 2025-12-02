/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationAccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationAccelerationSensor3DOF::SerializationAccelerationSensor3DOF(const std::string& name, const SensorType sensorType) :
	Device(name, deviceTypeSerializationAccelerationSensor3DOF(sensorType)),
	Measurement(name, deviceTypeSerializationAccelerationSensor3DOF(sensorType)),
	Sensor(name, deviceTypeSerializationAccelerationSensor3DOF(sensorType)),
	AccelerationSensor3DOF(name, sensorType),
	SerializationSensor(name, deviceTypeSerializationAccelerationSensor3DOF(sensorType))
{
	// nothing to do here
}

SerializationAccelerationSensor3DOF::~SerializationAccelerationSensor3DOF()
{
	// nothing to do here
}

void SerializationAccelerationSensor3DOF::forwardSampleEvent(Acceleration3DOFSample::Measurements&& measurements, const Timestamp& timestamp, Metadata&& metadata)
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

	postNewSample(SampleRef(new Acceleration3DOFSample(timestamp, std::move(objectIds), std::move(measurements), std::move(metadata))));
}

}

}

}
