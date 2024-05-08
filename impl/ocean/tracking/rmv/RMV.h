/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RMV_H
#define META_OCEAN_TRACKING_RMV_RMV_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * @ingroup tracking
 * @defgroup trackingrmv Ocean RMV Tracking Library
 * @{
 * The Ocean RMV Tracker Library implements a tracker based on the Random Model Variation tracking technology.
 * Instead of using correspondences between 2D/2D or 2D/3D feature points this tracker is based on random modification of a model (the camera pose) and does not use any appearance information of a feature point (but the location).
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Tracking::RMV Namespace of the RMV Tracker library.<p>
 * The Namespace Ocean::Tracking::RMV is used in the entire Ocean RMV Tracker Library.
 */

// Defines OCEAN_TRACKING_RMV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_RMV_EXPORT
		#define OCEAN_TRACKING_RMV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_RMV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_RMV_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RMV_H
