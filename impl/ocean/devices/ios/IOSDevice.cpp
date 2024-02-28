// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/ios/IOSDevice.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSDevice::IOSDevice(const std::string& name, const DeviceType type) :
	Device(name, type),
	sensorIsStarted(false),
	sensorFirstIOSEventTimestamp(0),
	sensorFirstUnixEventTimestamp(false)
{
	// nothing to do here
}

const std::string& IOSDevice::library() const
{
	return nameIOSLibrary();
}

}

}

}
