// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/rmv/RMVDevice.h"
#include "ocean/devices/rmv/RMVFactory.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

RMVDevice::RMVDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& RMVDevice::library() const
{
	static const std::string deviceStaticLibraryName(nameRMVLibrary());
	return deviceStaticLibraryName;
}

}

}

}
