/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/mapbuilding/MapBuildingFactory.h"
#include "ocean/devices/mapbuilding/OnDeviceMapCreatorTracker6DOF.h"
#include "ocean/devices/mapbuilding/OnDeviceRelocalizerTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

MapBuildingFactory::MapBuildingFactory() :
	Factory(nameMapBuildingLibrary())
{
	registerDevice(OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF(), OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF(), InstanceFunction::createStatic(&MapBuildingFactory::createTracker6DOF));
	registerDevice(OnDeviceMapCreatorTracker6DOF::deviceNameOnDeviceMapCreatorTracker6DOF(), OnDeviceMapCreatorTracker6DOF::deviceTypeOnDeviceMapCreatorTracker6DOF(), InstanceFunction::createStatic(&MapBuildingFactory::createTracker6DOF));
}

bool MapBuildingFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new MapBuildingFactory()));
}

bool MapBuildingFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameMapBuildingLibrary());
}

Device* MapBuildingFactory::createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	if (name == OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF())
	{
		ocean_assert_and_suppress_unused(deviceType == OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF(), deviceType);

		return new OnDeviceRelocalizerTracker6DOF();
	}
	else if (name == OnDeviceMapCreatorTracker6DOF::deviceNameOnDeviceMapCreatorTracker6DOF())
	{
		ocean_assert(deviceType == OnDeviceMapCreatorTracker6DOF::deviceTypeOnDeviceMapCreatorTracker6DOF());

		return new OnDeviceMapCreatorTracker6DOF();
	}

	ocean_assert(false && "Invalid tracker!");
	return nullptr;
}

}

}

}
