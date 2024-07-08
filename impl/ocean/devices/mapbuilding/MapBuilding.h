/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_H
#define META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

/**
 * @ingroup devices
 * @defgroup devicesmapbuilding Ocean Devices MapBuilding Library
 * @{
 * The Ocean Devices MapBuilding Library provides feature-based tracking in the context of map building.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::MapBuilding Namespace of the Devices MapBuilding library.<p>
 * The Namespace Ocean::Devices::MapBuilding is used in the entire Ocean Devices MapBuilding Library.
 */

// Defines OCEAN_DEVICES_MAPBUILDING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_MAPBUILDING_EXPORT
		#define OCEAN_DEVICES_MAPBUILDING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_MAPBUILDING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_MAPBUILDING_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesmapbuilding
 */
	OCEAN_DEVICES_MAPBUILDING_EXPORT std::string nameMapBuildingLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this MapBuilding device library at the global device manager.
 * This function calls MapBuildingFactory::registerFactory() only.
 * @ingroup devicesmapbuilding
 */
void registerMapBuildingLibrary();

/**
 * Unregisters this MapBuilding device library at the global device manager.
 * This function calls MapBuildingactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesmapbuilding
 */
bool unregisterMapBuildingLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_MAPBUILDING_MAP_BUILDING_H
