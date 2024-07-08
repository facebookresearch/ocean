/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
