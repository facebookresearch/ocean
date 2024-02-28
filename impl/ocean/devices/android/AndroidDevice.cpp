// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/android/AndroidDevice.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidDevice::AndroidDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& AndroidDevice::library() const
{
	static const std::string staticLibraryName(nameAndroidLibrary());
	return staticLibraryName;
}

}

}

}
