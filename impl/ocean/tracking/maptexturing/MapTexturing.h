/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_MAP_TEXTURING_H
#define META_OCEAN_TRACKING_MAPTEXTURING_MAP_TEXTURING_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * @ingroup tracking
 * @defgroup trackingmaptexturing Ocean Map Texturing Tracking Library
 * @{
 * The Ocean Map Texturing Tracking Library implements functions to manage and generate textures for maps.
 * @}
 */

/**
 * @namespace Ocean::Tracking::MapBuilding Namespace of the Map Texturing Tracking library.<p>
 * The Namespace Ocean::Tracking::MapBuilding is used in the entire Ocean Map Texturing Tracking Library.
 */

// Defines OCEAN_TRACKING_MAPTEXTURING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_MAPTEXTURING_EXPORT
		#define OCEAN_TRACKING_MAPTEXTURING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_MAPTEXTURING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_MAPTEXTURING_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_MAP_TEXTURING_H
