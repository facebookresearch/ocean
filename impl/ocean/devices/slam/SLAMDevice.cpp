/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/SLAMDevice.h"
#include "ocean/devices/slam/SLAMFactory.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

SLAMDevice::SLAMDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& SLAMDevice::library() const
{
	static const std::string deviceStaticLibraryName(nameSLAMLibrary());
	return deviceStaticLibraryName;
}

}

}

}
