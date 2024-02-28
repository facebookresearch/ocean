// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_BLOB_BLOB_H
#define META_OCEAN_TRACKING_BLOB_BLOB_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * @ingroup tracking
 * @defgroup trackingblob Ocean Blob Tracker Library
 * @{
 * The Ocean Blob Tracker Library implements a 6DOF computer vision tracker based on Blob features.
 * The implementation is highly optimized and supports multi-core cpus.<br>
 * As tracking pattern a feature map is used providing the reference features allowing the pose calculation.<br>
 * @see FeatureTracker6DOF, FeatureMap.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Blob Namespace of the Blob Tracker library.<p>
 * The Namespace Ocean::Tracking::Blob is used in the entire Ocean Blob Tracker Library.
 */

// Defines OCEAN_TRACKING_BLOB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_BLOB_EXPORT
		#define OCEAN_TRACKING_BLOB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_BLOB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_BLOB_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_BLOB_H
