/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_MAP_BUILDING_H
#define META_OCEAN_TRACKING_MAPBUILDING_MAP_BUILDING_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * @ingroup tracking
 * @defgroup trackingmapbuilding Ocean Map Building Tracking Library
 * @{
 * The Ocean Map Building Tracking Library implements functionalities to build map for tracking and relocalization.
 * @}
 */

/**
 * @namespace Ocean::Tracking::MapBuilding Namespace of the Map Building Tracking library.<p>
 * The Namespace Ocean::Tracking::MapBuilding is used in the entire Ocean Map Building Tracking Library.
 */

// Defines OCEAN_TRACKING_MAPBUILDING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_MAPBUILDING_EXPORT
		#define OCEAN_TRACKING_MAPBUILDING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_MAPBUILDING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_MAPBUILDING_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_MAP_BUILDING_H
