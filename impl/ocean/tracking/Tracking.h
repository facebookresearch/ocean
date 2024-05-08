/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_TRACKING_H
#define META_OCEAN_TRACKING_TRACKING_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

/**
 * @defgroup tracking Ocean Tracking Library
 * @{
 * The Ocean Tracker Library is the base class for all tracker implementations.<br>
 * In contrast to the computer vision library this library does not implement e.g. feature or corner detectors but implement the application of such features.<br>
 * This library provides several sub libraries applying individual e.g. feature approaches.
 * @}
 */

/**
 * @namespace Ocean::Tracking Namespace of the Tracker library.<p>
 * The Namespace Ocean::Tracking is used in the entire Ocean Tracker Library.
 */

/**
 * Definition of a 2D image point.
 * @ingroup tracking
 */
typedef Vector2 ImagePoint;

/**
 * Definition of a 3D object point.
 * @ingroup tracking
 */
typedef Vector3 ObjectPoint;

/**
 * Definition of a vector holding 2D image points.
 * @ingroup tracking
 */
typedef std::vector<ImagePoint> ImagePoints;

/**
 * Definition of a vector holding 3D object points.
 * @ingroup tracking
 */
typedef std::vector<ObjectPoint> ObjectPoints;

/**
 * Definition of a vector holding object points.
 * @ingroup tracking
 */
typedef std::vector<ObjectPoints> ObjectPointsVector;

/**
 * Definition of a vector holding image points.
 * @ingroup tracking
 */
typedef std::vector<ImagePoints> ImagePointsVector;


// Defines OCEAN_TRACKING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_EXPORT
		#define OCEAN_TRACKING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_EXPORT
#endif

}

}

#endif // META_OCEAN_TRACKING_TRACKING_H
