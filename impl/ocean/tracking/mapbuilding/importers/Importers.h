// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_IMPORTERS_H
#define META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_IMPORTERS_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

/**
 * @ingroup trackingmapbuilding
 * @defgroup trackingmapbuildingimporters Ocean Map Building Importers Tracking Library
 * @{
 * The Ocean Map Building Importers Tracking Library implements functionalities to import maps.
 * @}
 */

/**
 * @namespace Ocean::Tracking::MapBuilding::Importers Namespace of the Map Building Importers Tracking library.<p>
 * The Namespace Ocean::Tracking::MapBuilding::Importers is used in the entire Ocean Map Building Importers Tracking Library.
 */

// Defines OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT
		#define OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_IMPORTERS_H
