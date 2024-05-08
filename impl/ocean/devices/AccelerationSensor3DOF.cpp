/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/AccelerationSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

AccelerationSensor3DOF::Acceleration3DOFSample::Acceleration3DOFSample(const Timestamp& timestamp, const ObjectIds& objectIds, const Measurements& measurements, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	measurements_(measurements)
{
	// nothing to do here
}

AccelerationSensor3DOF::Acceleration3DOFSample::Acceleration3DOFSample(const Timestamp& timestamp, ObjectIds&& objectIds, Measurements&& measurements, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	measurements_(std::move(measurements))
{
	// nothing to do here
}

AccelerationSensor3DOF::AccelerationSensor3DOF(const std::string& name, const SensorType sensorType) :
	Device(name, deviceTypeAccelerationSensor3DOF(sensorType)),
	Measurement(name, deviceTypeAccelerationSensor3DOF(sensorType)),
	Sensor(name, deviceTypeAccelerationSensor3DOF(sensorType))
{
	// nothing to do here
}

AccelerationSensor3DOF::~AccelerationSensor3DOF()
{
	// nothing to do here
}

}

}
