/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
