// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudDevice.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudDevice::CloudDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& CloudDevice::library() const
{
	return nameCloudLibrary();
}

}

}

}
