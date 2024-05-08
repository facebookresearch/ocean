/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_UTILITIES_H
#define META_OCEAN_TRACKING_POINT_UTILITIES_H

#include "ocean/tracking/point/Point.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements utilities function for the point tracking library.
 * @ingroup trackingpoint
 */
class OCEAN_TRACKING_POINT_EXPORT Utilities
{
	public:

		/**
		 * Determines an initial 3D position from several 3D rays that start at the camera center and intersect individual image points.
		 * @param rays Rays for that a best matching pose has to be determined
		 * @param numberRays Number of provided rays, with range [2, infinity)
		 * @param objectPoint Resulting object point
		 * @param checkFrontPosition True, if the resulting point is verified to be in front of all viewing rays (by application of the ray direction)
		 * @return True, if succeeded
		 */
		static bool determineInitialObjectPoint(const Line3* rays, const size_t numberRays, Vector3& objectPoint, const bool checkFrontPosition = true);
};

}

}

}

#endif // META_OCEAN_TRACKING_POINT_UTILITIES_H
