// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_PLANE_PLANE_H
#define META_OCEAN_TRACKING_PLANE_PLANE_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Plane
{

/**
 * @ingroup tracking
 * @defgroup trackingplane Ocean library implementing the PlaneTracker
 * @{
 * The Ocean Plane Tracker Library implements a visual-based tracker providing a 6-DOF camera pose in relations to planes.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Plane Namespace of the PlaneTracker library.<p>
 * The Namespace Ocean::Tracking::Plane is used in the entire Ocean PlaneTracker Library.
 */

// Defines OCEAN_TRACKING_PLANE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_PLANE_EXPORT
		#define OCEAN_TRACKING_PLANE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_PLANE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_PLANE_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_PLANE_PLANE_H
