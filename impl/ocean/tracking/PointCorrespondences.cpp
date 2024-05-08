/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/PointCorrespondences.h"

#include "ocean/base/Median.h"
#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Tracking
{

unsigned int PointCorrespondences::determineValidCorrespondencesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const Geometry::ImagePoint* imagePoints, const size_t correspondences, const bool distortImagePoints, const Scalar sqrPixelError, Indices32* validCorrespondences)
{
	ocean_assert(objectPoints && imagePoints);
	ocean_assert(sqrPixelError >= 0);

	unsigned int result = 0u;

	if (distortImagePoints && pinholeCamera.hasDistortionParameters())
	{
		for (unsigned int n = 0u; n < correspondences; ++n)
			if (pinholeCamera.projectToImageIF<true, true>(invertedFlippedExtrinsic, objectPoints[n]).sqrDistance(imagePoints[n]) <= sqrPixelError)
			{
				++result;

				if (validCorrespondences)
					validCorrespondences->push_back(n);
			}
	}
	else
	{
		for (unsigned int n = 0u; n < correspondences; ++n)
			if (pinholeCamera.projectToImageIF<false, true>(invertedFlippedExtrinsic, objectPoints[n]).sqrDistance(imagePoints[n]) <= sqrPixelError)
			{
				++result;

				if (validCorrespondences)
					validCorrespondences->push_back(n);
			}
	}

	return result;
}

void PointCorrespondences::removeInvalidCorrespondencesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints, const bool distortImagePoints, const Scalar sqrPixelError)
{
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(sqrPixelError >= 0);

	Geometry::ObjectPoints tmpObjectPoints;
	tmpObjectPoints.reserve(objectPoints.size());

	Geometry::ImagePoints tmpImagePoints;
	tmpImagePoints.reserve(imagePoints.size());

	if (distortImagePoints && pinholeCamera.hasDistortionParameters())
	{
		for (size_t n = 0; n < objectPoints.size(); ++n)
			if (pinholeCamera.projectToImageIF<true, true>(invertedFlippedExtrinsic, objectPoints[n]).sqrDistance(imagePoints[n]) <= sqrPixelError)
			{
				tmpObjectPoints.push_back(objectPoints[n]);
				tmpImagePoints.push_back(imagePoints[n]);
			}
	}
	else
	{
		for (size_t n = 0; n < objectPoints.size(); ++n)
			if (pinholeCamera.projectToImageIF<false, true>(invertedFlippedExtrinsic, objectPoints[n]).sqrDistance(imagePoints[n]) <= sqrPixelError)
			{
				tmpObjectPoints.push_back(objectPoints[n]);
				tmpImagePoints.push_back(imagePoints[n]);
			}
	}

	objectPoints = std::move(tmpObjectPoints);
	imagePoints = std::move(tmpImagePoints);
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar searchWindowRadius, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(searchWindowRadius > 0);

	Scalar distance0, distance1;
	unsigned int index0, index1;

	if (candidateUseCounter)
		*candidateUseCounter = Indices32(numberCandidatePoints, 0u);

	RedundantCorrespondences result;

	for (unsigned int i = 0u; i < numberImagePoints; ++i)
	{
		const Geometry::ImagePoint& imagePoint = imagePoints[i];

		distance0 = Numeric::maxValue();
		distance1 = Numeric::maxValue();

		index0 = (unsigned int)(-1);
		index1 = (unsigned int)(-1);

		const Scalar left = imagePoint.x() - searchWindowRadius;
		const Scalar right = imagePoint.x() + searchWindowRadius;
		const Scalar top = imagePoint.y() - searchWindowRadius;
		const Scalar bottom = imagePoint.y() + searchWindowRadius;

		for (unsigned int c = 0; c < numberCandidatePoints; ++c)
		{
			const Geometry::ImagePoint& candidatePoint = candidatePoints[c];

			if (candidatePoint.x() >= left && candidatePoint.x() <= right && candidatePoint.y() >= top && candidatePoint.y() <= bottom)
			{
				const Scalar sqrDistance = imagePoint.sqrDistance(candidatePoint);

				if (sqrDistance < distance0)
				{
					distance1 = distance0;
					index1 = index0;

					distance0 = sqrDistance;
					index0 = c;
				}
				else if (sqrDistance < distance1)
				{
					distance1 = sqrDistance;
					index1 = c;
				}
			}
		}

		if (index0 != (unsigned int)(-1))
		{
			result.push_back(RedundantCorrespondence(i, index0, distance0, index1, distance1));

			if (candidateUseCounter)
				(*candidateUseCounter)[index0]++;
		}
	}

	return result;
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar searchWindowRadius, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(searchWindowRadius > 0);

	const unsigned int horizontalBins = minmax(1u, (unsigned int)(Scalar(width) / (Scalar(2) * searchWindowRadius)), 20u);
	const unsigned int verticalBins = minmax(1u, (unsigned int)(Scalar(height) / (Scalar(2) * searchWindowRadius)), 20u);
	const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(candidatePoints, numberCandidatePoints, Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins));

	ocean_assert(distributionArray.horizontalBins() == horizontalBins);
	ocean_assert(distributionArray.verticalBins() == verticalBins);

	return determineNearestCandidates(imagePoints, numberImagePoints, candidatePoints, numberCandidatePoints, searchWindowRadius, distributionArray, candidateUseCounter);
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidatesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ObjectPoint* candidatePoints, const size_t numberCandidatePoints, const bool distortImagePoints, const Scalar searchWindowRadius, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);

	if (numberCandidatePoints == 0)
		return RedundantCorrespondences();

	// project all given candidate object points onto the image plane
	Geometry::ImagePoints candidateImagePoints(numberCandidatePoints);
	pinholeCamera.projectToImageIF<true>(invertedFlippedExtrinsic, candidatePoints, numberCandidatePoints, distortImagePoints, candidateImagePoints.data());

	return determineNearestCandidates(imagePoints, numberImagePoints, candidateImagePoints.data(), numberCandidatePoints, searchWindowRadius, candidateUseCounter);
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar searchWindowRadius, const Geometry::SpatialDistribution::DistributionArray& distributionCandidatePoints, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(searchWindowRadius > 0);

	Scalar distance0, distance1;
	unsigned int index0, index1;

	if (candidateUseCounter)
		*candidateUseCounter = Indices32(numberCandidatePoints, 0u);

	RedundantCorrespondences result;

	for (unsigned int i = 0u; i < numberImagePoints; ++i)
	{
		const Geometry::ImagePoint& imagePoint = imagePoints[i];

		const int xBin = distributionCandidatePoints.horizontalBin(imagePoint.x());
		const int yBin = distributionCandidatePoints.verticalBin(imagePoint.y());

		distance0 = Numeric::maxValue();
		distance1 = Numeric::maxValue();

		index0 = (unsigned int)(-1);
		index1 = (unsigned int)(-1);

		const Scalar left = imagePoint.x() - searchWindowRadius;
		const Scalar right = imagePoint.x() + searchWindowRadius;
		const Scalar top = imagePoint.y() - searchWindowRadius;
		const Scalar bottom = imagePoint.y() + searchWindowRadius;

		for (int y = max(0, yBin - 1); y <= min(yBin + 1, int(distributionCandidatePoints.verticalBins()) - 1); ++y)
		{
			for (int x = max(0, xBin - 1); x <= min(xBin + 1, int(distributionCandidatePoints.horizontalBins()) - 1); ++x)
			{
				const Indices32& indices = distributionCandidatePoints(x, y);

				for (Indices32::const_iterator candidateIndex = indices.begin(); candidateIndex != indices.end(); ++candidateIndex)
				{
					if (*candidateIndex < numberCandidatePoints)
					{
						const Geometry::ImagePoint& candidatePoint = candidatePoints[*candidateIndex];

						if (candidatePoint.x() >= left && candidatePoint.x() <= right && candidatePoint.y() >= top && candidatePoint.y() <= bottom)
						{
							const Scalar sqrDistance = imagePoint.sqrDistance(candidatePoint);

							if (sqrDistance < distance0)
							{
								distance1 = distance0;
								index1 = index0;

								distance0 = sqrDistance;
								index0 = *candidateIndex;
							}
							else if (sqrDistance < distance1)
							{
								distance1 = sqrDistance;
								index1 = *candidateIndex;
							}
						}
					}
				}
			}
		}

		if (index0 != (unsigned int)(-1))
		{
			result.push_back(RedundantCorrespondence(i, index0, distance0, index1, distance1));

			if (candidateUseCounter)
				(*candidateUseCounter)[index0]++;
		}
	}

	return result;
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const Line2* candidateLines, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar searchWindowRadius, const Scalar maximalLineSqrDistance, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(searchWindowRadius > 0);

	const unsigned int horizontalBins = minmax(1u, (unsigned int)(Scalar(width) / (Scalar(2) * searchWindowRadius)), 20u);
	const unsigned int verticalBins = minmax(1u, (unsigned int)(Scalar(height) / (Scalar(2) * searchWindowRadius)), 20u);
	const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(candidatePoints, numberCandidatePoints, Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins));

	ocean_assert(distributionArray.horizontalBins() == horizontalBins);
	ocean_assert(distributionArray.verticalBins() == verticalBins);

	return determineNearestCandidates(imagePoints, numberImagePoints, candidatePoints, candidateLines, numberCandidatePoints, searchWindowRadius, maximalLineSqrDistance, distributionArray, candidateUseCounter);
}

PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const Line2* candidateLines, const size_t numberCandidatePoints, const Scalar searchWindowRadius, const Scalar maximalLineSqrDistance, const Geometry::SpatialDistribution::DistributionArray& distributionCandidatePoints, Indices32* candidateUseCounter)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(searchWindowRadius > 0);

	Scalar distance0, distance1;
	unsigned int index0, index1;

	if (candidateUseCounter)
		*candidateUseCounter = Indices32(numberCandidatePoints, 0u);

	RedundantCorrespondences result;

	for (unsigned int n = 0; n < numberImagePoints; ++n)
	{
		const Geometry::ImagePoint& imagePoint = imagePoints[n];

		const int xBin = distributionCandidatePoints.horizontalBin(imagePoint.x());
		const int yBin = distributionCandidatePoints.verticalBin(imagePoint.y());

		distance0 = Numeric::maxValue();
		distance1 = Numeric::maxValue();

		index0 = (unsigned int)(-1);
		index1 = (unsigned int)(-1);

		const Scalar left = imagePoint.x() - searchWindowRadius;
		const Scalar right = imagePoint.x() + searchWindowRadius;
		const Scalar top = imagePoint.y() - searchWindowRadius;
		const Scalar bottom = imagePoint.y() + searchWindowRadius;

		for (int y = max(0, yBin - 1); y <= min(yBin + 1, int(distributionCandidatePoints.verticalBins()) - 1); ++y)
			for (int x = max(0, xBin - 1); x <= min(xBin + 1, int(distributionCandidatePoints.horizontalBins()) - 1); ++x)
			{
				const Indices32& indices = distributionCandidatePoints(x, y);

				for (Indices32::const_iterator i = indices.begin(); i != indices.end(); ++i)
					if (*i < numberCandidatePoints)
					{
						const Geometry::ImagePoint& candidatePoint = candidatePoints[*i];

						if (candidatePoint.x() >= left && candidatePoint.x() <= right && candidatePoint.y() >= top && candidatePoint.y() <= bottom)
						{
							if (!candidateLines[*i].isValid() || candidateLines[*i].sqrDistance(imagePoint) <= maximalLineSqrDistance)
							{
								const Scalar sqrDistance = imagePoint.sqrDistance(candidatePoint);

								if (sqrDistance < distance0)
								{
									distance1 = distance0;
									index1 = index0;

									distance0 = sqrDistance;
									index0 = *i;
								}
								else if (sqrDistance < distance1)
								{
									distance1 = sqrDistance;
									index1 = *i;
								}
							}
						}
					}
			}

		if (index0 != (unsigned int)(-1))
		{
			result.push_back(RedundantCorrespondence(n, index0, distance0, index1, distance1));

			if (candidateUseCounter)
				(*candidateUseCounter)[index0]++;
		}
	}

	return result;
}

Indices32 PointCorrespondences::filterValidCorrespondences(const Vector2* firstPoints, const Vector2* secondPoints, const size_t numberPoints, const Scalar thresholdFactor)
{
	ocean_assert(firstPoints && secondPoints);
	ocean_assert(numberPoints != 0);

	ocean_assert(thresholdFactor >= 0);

	Scalars sqrErrors(numberPoints);

	for (size_t n = 0; n < numberPoints; ++n)
		sqrErrors[n] = firstPoints[n].sqrDistance(secondPoints[n]);

	Scalars tmpSqrErrors(sqrErrors);
	const Scalar threshold = Median::median(tmpSqrErrors.data(), tmpSqrErrors.size()) * thresholdFactor;

	Indices32 result;
	result.reserve(numberPoints);

	for (unsigned int n = 0u; n < (unsigned int)numberPoints; ++n)
		if (sqrErrors[n] <= threshold)
			result.push_back(n);

	return result;
}

Indices32 PointCorrespondences::filterValidCorrespondences(const Vectors2& firstPoints, const Vectors2& secondPoints, const Indices32& subsetIndices, const Scalar thresholdFactor)
{
	ocean_assert(firstPoints.size() && secondPoints.size());
	ocean_assert(subsetIndices.size() <= firstPoints.size());

	ocean_assert(thresholdFactor >= 0);

	Scalars sqrErrors;
	sqrErrors.reserve(subsetIndices.size());

	for (Indices32::const_iterator i = subsetIndices.begin(); i != subsetIndices.end(); ++i)
	{
		ocean_assert(*i < firstPoints.size());
		ocean_assert(*i < secondPoints.size());

		sqrErrors.push_back(firstPoints[*i].sqrDistance(secondPoints[*i]));
	}

	Scalars tmpSqrErrors(sqrErrors);
	const Scalar threshold = Median::median(tmpSqrErrors.data(), tmpSqrErrors.size()) * thresholdFactor;

	Indices32 result;
	result.reserve(sqrErrors.size());

	for (size_t n = 0; n < sqrErrors.size(); ++n)
		if (sqrErrors[n] <= threshold)
		{
			ocean_assert(n < subsetIndices.size());
			result.push_back(subsetIndices[n]);
		}

	return result;
}

}

}
