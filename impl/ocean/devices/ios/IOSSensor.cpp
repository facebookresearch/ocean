// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/ios/IOSSensor.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSSensor::IOSSensor(const std::string& name, const DeviceType type) :
	Device(name, type),
	IOSDevice(name, type),
	Measurement(name, type),
	Sensor(name, type)
{
	sensorObjectId_ = addUniqueObjectId(name);
}

}

}

}
