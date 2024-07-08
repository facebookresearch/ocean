/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
