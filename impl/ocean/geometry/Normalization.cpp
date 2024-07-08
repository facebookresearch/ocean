/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Normalization.h"

namespace Ocean
{

namespace Geometry
{

SquareMatrix3 Normalization::calculateNormalizedPoints(Vector2* points, const size_t number, SquareMatrix3* points_T_normalizedPoints)
{
	if (number == 0)
	{
		if (points_T_normalizedPoints != nullptr)
		{
			points_T_normalizedPoints->toIdentity();
		}

		return SquareMatrix3(true);
	}

	ocean_assert(points != nullptr);

	// determine mean point
	Vector2 normalizationMeanPoint(0, 0);
	for (size_t n = 0; n < number; ++n)
	{
		normalizationMeanPoint += points[n];
	}

	ocean_assert(number != 0);
	normalizationMeanPoint /= Scalar(number);


	// determine scale
	Scalar normalizationScale = Scalar(0);
	for (size_t n = 0; n < number; ++n)
	{
		normalizationScale += (points[n] - normalizationMeanPoint).sqr();
	}

	const Scalar invNormalizationScale = Numeric::sqrt(normalizationScale / Scalar(number) * Scalar(0.5));

	if (Numeric::isEqualEps(invNormalizationScale))
	{
		// we will not be able to determine the inverted normalization so that we simply cannot normalize the data set
		if (points_T_normalizedPoints != nullptr)
		{
			points_T_normalizedPoints->toIdentity();
		}

		return SquareMatrix3(true);
	}

	normalizationScale = Scalar(1) / invNormalizationScale;


	// apply normalization
	for (size_t n = 0; n < number; ++n)
	{
		points[n] = Vector2(normalizationScale * (points[n].x() - normalizationMeanPoint.x()), normalizationScale * (points[n].y() - normalizationMeanPoint.y()));
	}

#ifdef OCEAN_DEBUG
	if constexpr (std::is_same<Scalar, double>::value)
	{
		Scalar meanSquareDistance = 0;

		for (size_t n = 0; n < number; ++n)
		{
			meanSquareDistance += points[n].sqr();
		}

		meanSquareDistance /= Scalar(number);

		// sqrt(meanSquareDistance =!= sqrt(2))
		ocean_assert(Numeric::isEqual(meanSquareDistance, 2));
	}
#endif // OCEAN_DEBUG

	if (points_T_normalizedPoints != nullptr)
	{
		*points_T_normalizedPoints = SquareMatrix3(invNormalizationScale, 0, 0, 0, invNormalizationScale, 0, normalizationMeanPoint.x(), normalizationMeanPoint.y(), 1);
	}

	return SquareMatrix3(normalizationScale, 0, 0, 0, normalizationScale, 0, - normalizationMeanPoint.x() * normalizationScale, - normalizationMeanPoint.y() * normalizationScale, 1);
}

}

}
