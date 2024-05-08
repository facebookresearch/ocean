// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_UVTEXTUREMAPPING_UVTEXTURE_MAPPING_H
#define META_OCEAN_TRACKING_UVTEXTUREMAPPING_UVTEXTURE_MAPPING_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

/**
 * @ingroup tracking
 * @defgroup trackinguvtexturemapping Ocean UV texture mapping Library
 * @{
 * The Ocean UV texture mapping library defines mapping functions from 3D geometry to 2D textures, for use in tracking.
 * @}
 */

/**
 * @namespace Ocean::Tracking::UVTextureMapping Namespace of the UV texture mapping library.<p>
 * The Namespace Ocean::Tracking::UVTextureMapping is used in the entire Ocean UV texture mapping library.
 */

// Defines OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT
		#define OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_UVTEXTUREMAPPING_UVTEXTURE_MAPPING_H
