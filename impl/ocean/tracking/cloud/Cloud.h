// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_CLOUD_CLOUD_H
#define META_OCEAN_TRACKING_CLOUD_CLOUD_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace Cloud
{

/**
 * @ingroup tracking
 * @defgroup trackingcloud Ocean Cloud Tracker Library
 * @{
 * The Ocean Cloud Tracker Library provides low level access to cloud relocalization features.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Cloud Namespace of the Cloud Tracker library.<p>
 * The Namespace Ocean::Tracking::Cloud is used in the entire Ocean Cloud Tracker Library.
 */

// Defines OCEAN_TRACKING_CLOUD_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_CLOUD_EXPORT
		#define OCEAN_TRACKING_CLOUD_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_CLOUD_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_CLOUD_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_CLOUD_CLOUD_H
