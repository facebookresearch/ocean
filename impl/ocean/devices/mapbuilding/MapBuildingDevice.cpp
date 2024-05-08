/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/mapbuilding/MapBuildingDevice.h"
#include "ocean/devices/mapbuilding/MapBuildingFactory.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

MapBuildingDevice::MapBuildingDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& MapBuildingDevice::library() const
{
	static std::string name = nameMapBuildingLibrary();

	return name;
}

}

}

}
