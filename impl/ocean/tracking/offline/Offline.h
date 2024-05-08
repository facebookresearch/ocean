/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_OFFLINE_H
#define META_OCEAN_TRACKING_OFFLINE_OFFLINE_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

/**
 * @ingroup tracking
 * @defgroup trackingoffline Ocean Offline Tracking Library
 * @{
 * The Ocean Offline Tracker Library implements a 6DOF computer vision tracker that is designed for post processing.
 * Offline tracker are not expected to be applied in real-time applications as these trackers need the entire video information in-advance.<br>
 * An offline tracker applies several individual iterations to improve the tracking accuracy and robustness.<br>
 * Thus, a main feature of an offline tracker is that it uses the CV::FrameProviderInterface as input for the visual information.
 * @see FeatureTracker6DOF, FeatureMap.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Offline Namespace of the Offline Tracker library.<p>
 * The Namespace Ocean::Tracking::Offline is used in the entire Ocean Offline Tracker Library.
 */

// Defines OCEAN_TRACKING_OFFLINE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_OFFLINE_EXPORT
		#define OCEAN_TRACKING_OFFLINE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_OFFLINE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_OFFLINE_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_OFFLINE_H
