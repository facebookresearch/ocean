/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/GyroSensor3DOF.h"

namespace Ocean
{

namespace Devices
{

GyroSensor3DOF::Gyro3DOFSample::Gyro3DOFSample(const Timestamp& timestamp, const ObjectIds& objectIds, const Measurements& measurements) :
	Sample(timestamp, objectIds),
	measurements_(measurements)
{
	// nothing to do here
}

GyroSensor3DOF::Gyro3DOFSample::Gyro3DOFSample(const Timestamp& timestamp, ObjectIds&& objectIds, Measurements&& measurements) :
	Sample(timestamp, std::move(objectIds)),
	measurements_(std::move(measurements))
{
	// nothing to do here
}

GyroSensor3DOF::GyroSensor3DOF(const std::string& name, const SensorType sensorType) :
	Device(name, deviceTypeGyroSensor3DOF(sensorType)),
	Measurement(name, deviceTypeGyroSensor3DOF(sensorType)),
	Sensor(name, deviceTypeGyroSensor3DOF(sensorType))
{
	// nothing to do here
}

GyroSensor3DOF::~GyroSensor3DOF()
{
	// nothing to do here
}

}

}
