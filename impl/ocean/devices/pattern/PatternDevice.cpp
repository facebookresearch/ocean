/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/pattern/PatternDevice.h"
#include "ocean/devices/pattern/PatternFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

PatternDevice::PatternDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& PatternDevice::library() const
{
	static const std::string deviceStaticLibraryName(namePatternLibrary());
	return deviceStaticLibraryName;
}

}

}

}
