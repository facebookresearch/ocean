// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/mapbuilding/MapBuilding.h"
#include "ocean/devices/mapbuilding/MapBuildingFactory.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

std::string nameMapBuildingLibrary()
{
	return std::string("MapBuilding Tracker library");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerMapBuildingLibrary()
{
	MapBuildingFactory::registerFactory();
}

bool unregisterMapBuildingLibrary()
{
	return MapBuildingFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
