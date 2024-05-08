/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_PATTERN_PATTERN_H
#define META_OCEAN_TRACKING_PATTERN_PATTERN_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

/**
 * @ingroup tracking
 * @defgroup trackingpattern Ocean Pattern Tracking Interface
 * @{
 * The Ocean Pattern Tracker Library allows to track individual patterns.
 * You can use the PatternTracker6DOF class which is derived from VisualTracker,<br>
 * or you can use the actual implementation directly PatternTrackerCore6DOF if you want to get rid of the object oriented implementation.<br>
 *
 * The tracker provides camera poses for individual tracking patterns.<br>
 * Each pattern is connected with an id allowing to distinguish between the individual patterns.<br>
 * For each camera frame, the tracker determines pairs of pattern ids and camera poses (for all visible patterns).<br>
 * Each resulting camera pose transforms points defined in the camera coordinate system to points defined in the coordinate system of the corresponding pattern (pTc).<br>
 *
 * Each pattern has an own coordinate system with origin in the upper left corner of the pattern image (the pattern is expected to be located e.g., on a table - not on a wall).<br>
 * The x-axis is pointing along the horizontal border of the pattern (to the right),<br>
 * the y-axis is pointing upwards towards the observer,<br>
 * the z-axis is pointing along the vertical border of the pattern (downwards):
 * <pre>
 *       ^
 *     Y |
 *       |
 *       |
 *       O ---------> ............. (pattern top right)
 *      /        X               .
 *     / Z                      .
 *    v                        .
 *   .                        .
 *  .                        .
 * . ........................ (pattern bottom right)
 * (pattern bottom left)
 * </pre>
 *
 * The coordinate system of the camera is defined such that the observer/camera is looking into the negative z-space, the origin is in the center of the image.<br>
 * The x-axis points to the right of the camera, and the y-axis is identical to the up vector of the camera:<br>
 * <pre>
 * ................................. (camera image top right)
 * .                               .
 * .            ^                  .
 * .          Y |                  .
 * .            |                  .
 * .            |                  .
 * .            O --------->       .
 * .           /        X          .
 * .          / Z                  .
 * .         v                     .
 * .                               .
 * ................................. (camera image bottom right)
 * (camera image bottom left)
 * </pre>
 * @see PatternTracker6DOF, PatternTrackerCore6DOF.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Pattern Namespace of the Pattern Tracker library.<p>
 * The Namespace Ocean::Tracking::Pattern is used in the entire Ocean Pattern Tracker Library.
 */

// Defines OCEAN_TRACKING_PATTERN_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_PATTERN_EXPORT
		#define OCEAN_TRACKING_PATTERN_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_PATTERN_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_PATTERN_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_PATTERN_PATTERN_H
