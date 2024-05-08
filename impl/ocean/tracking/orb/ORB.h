/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_ORB_ORB_H
#define META_OCEAN_TRACKING_ORB_ORB_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

/**
 * @ingroup tracking
 * @defgroup trackingorb Ocean ORB Tracker Library
 * @{
 * The Ocean ORB Tracker Library implements a tracker based on ORB features.
 * The implementation is highly optimized and supports multi-core cpus.<br>
 * As tracking pattern a feature map is used providing the reference features allowing the pose calculation.<br>
 * @see FeatureMap.
 * @}
 */

/**
 * @namespace Ocean::Tracking::ORB Namespace of the ORB Tracker library.<p>
 * The Namespace Ocean::Tracking::ORB is used in the entire Ocean ORB Tracker Library.
 */

// Defines OCEAN_TRACKING_ORB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_ORB_EXPORT
		#define OCEAN_TRACKING_ORB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_ORB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_ORB_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_ORB_ORB_H
