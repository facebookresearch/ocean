// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/mapbuilding/Plugin.h"
#include "ocean/devices/mapbuilding/MapBuildingFactory.h"

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	return Ocean::Devices::MapBuilding::MapBuildingFactory::registerFactory();
}

bool pluginUnload()
{
	return Ocean::Devices::MapBuilding::MapBuildingFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_SHARED)
