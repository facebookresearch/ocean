/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/point/Utilities.h"

#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

bool Utilities::determineInitialObjectPoint(const Line3* rays, const size_t numberRays, Vector3& objectPoint, const bool checkFrontPosition)
{
	ocean_assert(rays && numberRays >= 2);

	Scalar minCosValue = Numeric::maxValue();
	unsigned int firstIndex = (unsigned int)(-1);
	unsigned int secondIndex = (unsigned int)(-1);

	for (size_t a = 0u; a < numberRays - 1; ++a)
		for (size_t b = a + 1u; b < numberRays; ++b)
		{
			ocean_assert(rays[a].hasUnitDirection());
			ocean_assert(rays[b].hasUnitDirection());

			const Scalar cosValue = rays[a].direction() * rays[b].direction();

			if (cosValue < minCosValue)
			{
				minCosValue = cosValue;
				firstIndex = (unsigned int)a;
				secondIndex = (unsigned int)b;
			}
		}

	ocean_assert(firstIndex != (unsigned int)(-1));
	ocean_assert(secondIndex != (unsigned int)(-1));

	if (minCosValue >= Scalar(0.99999390765779)) // Numeric::cos(Numeric::deg2rad(0.2)))
		return false;

	const bool result = rays[firstIndex].nearestPoint(rays[secondIndex], objectPoint);
	ocean_assert_and_suppress_unused(result, result);

	if (checkFrontPosition)
	{
		// check whether the object point is in front of every pose
		for (unsigned int n = 0; n < numberRays; ++n)
		{
			const Vector3 pointDirection(objectPoint - rays[n].point());

			if (rays[n].direction() * pointDirection < Numeric::eps())
				return false;
		}
	}

	return true;
}

}

}

}
