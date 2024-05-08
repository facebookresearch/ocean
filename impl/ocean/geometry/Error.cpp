/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Error.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Quaternion.h"

#include <algorithm>

namespace Ocean
{

namespace Geometry
{

void Error::determineValidParameters(const Scalar* parameters, const size_t number, const Scalar threshold, Indices32& validIndices)
{
	ocean_assert(parameters != nullptr);
	ocean_assert(validIndices.empty());

	validIndices.clear();
	validIndices.reserve(number);

	ocean_assert(NumericT<Index32>::isInsideValueRange(number));

	for (size_t n = 0; n < number; ++n)
	{
		if (parameters[n] <= threshold)
		{
			validIndices.emplace_back(Index32(n));
		}
	}
}

void Error::determineInvalidParameters(const Scalar* parameters, const size_t number, const Scalar threshold, Indices32& validIndices)
{
	ocean_assert(parameters != nullptr);
	ocean_assert(validIndices.empty());

	validIndices.clear();
	validIndices.reserve(number);

	ocean_assert(NumericT<Index32>::isInsideValueRange(number));

	for (size_t n = 0; n < number; ++n)
	{
		if (parameters[n] > threshold)
		{
			validIndices.emplace_back(Index32(n));
		}
	}
}

Scalar Error::determineAverageError(const Vectors2& firstPoints, const Vectors2& secondPoints, Vector2* errors, Scalar* sqrErrors)
{
	ocean_assert(!firstPoints.empty());
	ocean_assert(firstPoints.size() == secondPoints.size());

	if (firstPoints.empty())
	{
		return Scalar(0);
	}

	Scalar result = 0;

	if (errors != nullptr || sqrErrors != nullptr)
	{
		for (size_t n = 0; n < firstPoints.size(); ++n)
		{
			const Vector2 distance(secondPoints[n] - firstPoints[n]);
			const Scalar sqrDistance(distance.sqr());

			result += sqrDistance;

			if (errors != nullptr)
			{
				errors[n] = distance;
			}

			if (sqrErrors != nullptr)
			{
				sqrErrors[n] = sqrDistance;
			}
		}
	}
	else
	{
		for (size_t n = 0; n < firstPoints.size(); ++n)
		{
			result += firstPoints[n].sqrDistance(secondPoints[n]);
		}
	}

	return result / Scalar(firstPoints.size());
}

Scalar Error::determineAverageError(const SquareMatrix3& firstTransformation, const Vectors2& firstPoints, const SquareMatrix3& secondTransformation, const Vectors2& secondPoints)
{
	ocean_assert(!firstPoints.empty());
	ocean_assert(firstPoints.size() == secondPoints.size());

	if (firstPoints.empty())
	{
		return Scalar(0);
	}

	Scalar result = 0;

	for (size_t n = 0; n < firstPoints.size(); ++n)
	{
		result += (firstTransformation * firstPoints[n]).sqrDistance(secondTransformation * secondPoints[n]);
	}

	return result / Scalar(firstPoints.size());
}

Scalar Error::determineAverageError(const Vectors3& firstPoints, const Vectors3& secondPoints)
{
	ocean_assert(!firstPoints.empty());
	ocean_assert(firstPoints.size() == secondPoints.size());

	if (firstPoints.empty())
	{
		return Scalar(0);
	}

	Scalar result = 0;

	for (size_t n = 0; n < firstPoints.size(); ++n)
	{
		result += firstPoints[n].sqrDistance(secondPoints[n]);
	}

	return result / Scalar(firstPoints.size());
}

Scalar Error::determineError(const Vectors2& firstPoints, const Vectors2& secondPoints, Scalar& sqrAverageError, Scalar& sqrMinimalError, Scalar& sqrMaximalError)
{
	ocean_assert(!firstPoints.empty());
	ocean_assert(firstPoints.size() == secondPoints.size());

	if (firstPoints.empty())
	{
		sqrAverageError = 0;
		sqrMinimalError = 0;
		sqrMaximalError = 0;

		return 0;
	}

	Scalar totalError = 0;
	sqrAverageError = 0;
	sqrMinimalError = Numeric::maxValue();
	sqrMaximalError = 0;

	for (size_t n = 0; n < firstPoints.size(); ++n)
	{
		const Scalar sqrDistance(firstPoints[n].sqrDistance(secondPoints[n]));

		totalError += sqrDistance;

		if (sqrDistance < sqrMinimalError)
		{
			sqrMinimalError = sqrDistance;
		}

		if (sqrDistance > sqrMaximalError)
		{
			sqrMaximalError = sqrDistance;
		}
	}

	sqrAverageError = totalError / Scalar(firstPoints.size());

	return totalError;
}

Scalar Error::determineError(const Vectors3& firstPoints, const Vectors3& secondPoints, Scalar& sqrAverageError, Scalar& sqrMinimalError, Scalar& sqrMaximalError)
{
	ocean_assert(!firstPoints.empty());
	ocean_assert(firstPoints.size() == secondPoints.size());

	if (firstPoints.empty())
	{
		sqrAverageError = 0;
		sqrMinimalError = 0;
		sqrMaximalError = 0;

		return 0;
	}

	Scalar totalError = 0;
	sqrAverageError = 0;
	sqrMinimalError = Numeric::maxValue();
	sqrMaximalError = 0;

	for (size_t n = 0; n < firstPoints.size(); ++n)
	{
		const Scalar sqrDistance(firstPoints[n].sqrDistance(secondPoints[n]));

		totalError += sqrDistance;

		if (sqrDistance < sqrMinimalError)
		{
			sqrMinimalError = sqrDistance;
		}

		if (sqrDistance > sqrMaximalError)
		{
			sqrMaximalError = sqrDistance;
		}
	}

	sqrAverageError = totalError / Scalar(firstPoints.size());

	return totalError;
}

bool Error::posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Vector3& maxTranslationOffset, const Scalar maxOrientationOffset)
{
	const Vector3 poseFirstPosition(poseFirst.translation());
	const Quaternion poseFirstOrientation(poseFirst.rotation());

	const Vector3 poseSecondPosition(poseSecond.translation());
	const Quaternion poseSecondOrientation(poseSecond.rotation());

	const Scalar maxOrientationOffsetCos2(Numeric::cos(maxOrientationOffset * Scalar(0.5)));

	return Numeric::abs(poseFirstPosition.x() - poseSecondPosition.x()) <= maxTranslationOffset.x()
						&& Numeric::abs(poseFirstPosition.y() - poseSecondPosition.y()) <= maxTranslationOffset.y()
						&& Numeric::abs(poseFirstPosition.z() - poseSecondPosition.z()) <= maxTranslationOffset.z()
						&& poseFirstOrientation.cos2(poseSecondOrientation) >= maxOrientationOffsetCos2;
}

Scalar Error::determineCameraError(const PinholeCamera& pinholeCamera, const Vector2* normalizedObjectPoints, const Vector2* imagePoints, const size_t correspondences, const bool useDistortionParameters, Vector2* errors, Scalar* sqrErrors)
{
	if (correspondences == 0)
	{
		return 0;
	}

	ocean_assert(normalizedObjectPoints != nullptr && imagePoints != nullptr);

	Scalar sqrAveragePixelError = 0;

	for (unsigned int n = 0; n < correspondences; ++n)
	{
		const Vector2 imagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(normalizedObjectPoints[n], useDistortionParameters));
		const Vector2& realImagePoint = imagePoints[n];

		const Vector2 difference(imagePoint - realImagePoint);
		const Scalar sqrPixelError = difference.sqr();

		if (errors != nullptr)
		{
			errors[n] = difference;
		}

		if (sqrErrors != nullptr)
		{
			sqrErrors[n] = sqrPixelError;
		}

		sqrAveragePixelError += sqrPixelError;
	}

	return sqrAveragePixelError / Scalar(correspondences);
}

void Error::determineCameraError(const PinholeCamera& pinholeCamera, const Vector2* normalizedObjectPoints, const Vector2* imagePoints, const size_t correspondences, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError)
{
	ocean_assert(normalizedObjectPoints && imagePoints);

	sqrAveragePixelError = 0;
	sqrMinimalPixelError = Numeric::maxValue();
	sqrMaximalPixelError = 0;

	if (correspondences == 0)
	{
		return;
	}

	for (unsigned int n = 0; n < correspondences; ++n)
	{
		const Vector2 imagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(normalizedObjectPoints[n], useDistortionParameters));
		const Vector2& realImagePoint = imagePoints[n];

		const Vector2 difference(imagePoint - realImagePoint);
		const Scalar pixelError = difference.sqr();

		sqrAveragePixelError += pixelError;
		sqrMinimalPixelError = min(sqrMinimalPixelError, pixelError);
		sqrMaximalPixelError = max(sqrMaximalPixelError, pixelError);
	}

	sqrAveragePixelError /= Scalar(correspondences);
}

}

}
