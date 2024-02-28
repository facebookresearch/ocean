// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/VrApiDevice.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

VrApiDevice::VrApiDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& VrApiDevice::library() const
{
	static const std::string staticLibraryName(nameQuestVrApiLibrary());
	return staticLibraryName;
}

}

}

}

}
