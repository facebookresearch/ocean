/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_POINT_H
#define META_OCEAN_TRACKING_POINT_POINT_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * @ingroup tracking
 * @defgroup trackingpoint Ocean Point Tracking Interface
 * @{
 * The Ocean Point Tracker Library allows to simply track points between several frames.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Point Namespace of the Point Tracker library.<p>
 * The Namespace Ocean::Tracking::Point is used in the entire Ocean Point Tracker Library.
 */

// Defines OCEAN_TRACKING_POINT_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_POINT_EXPORT
		#define OCEAN_TRACKING_POINT_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_POINT_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_POINT_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_POINT_POINT_H
