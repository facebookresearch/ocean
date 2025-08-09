/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/base/Subset.h"

namespace Ocean
{

namespace Geometry
{

SpatialDistribution::DistributionArray::DistributionArray(const DistributionArray& distributionArray, const bool copyNeighborhood8) :
	Array(distributionArray),
	indexGroups_(distributionArray.indexGroups_),
	hasCopiedNeighborhood8_(false)
{
	if (copyNeighborhood8)
	{
		hasCopiedNeighborhood8_ = true;

		for (unsigned int vertical = 0u; vertical < verticalBins_; ++vertical)
		{
			for (unsigned int horizontal = 0u; horizontal < horizontalBins_; ++horizontal)
			{
				Indices32& resultBin = (*this)(horizontal, vertical);

				// we iterate over the 8-neighborhood and add all indices to `resultBin`

				for (unsigned int y = (unsigned int)(max(0, int(vertical) - 1)); y < min(vertical + 2u, verticalBins_); ++y)
				{
					for (unsigned int x = (unsigned int)(max(0, int(horizontal) - 1)); x < min(horizontal + 2u, horizontalBins_); ++x)
					{
						if (x != horizontal || y != vertical)
						{
							const Indices32& localIndices = distributionArray(x, y);

							resultBin.insert(resultBin.cend(), localIndices.cbegin(), localIndices.cend());
						}
					}
				}
			}
		}
	}
}

Indices32 SpatialDistribution::DistributionArray::indicesNeighborhood9(const unsigned int horizontal, const unsigned int vertical) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	size_t number = 0;
	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			number += (*this)(x, y).size();
		}
	}

	Indices32 result;
	result.reserve(number);

	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			const Indices32& indices = (*this)(x, y);
			result.insert(result.end(), indices.begin(), indices.end());
		}
	}

	return result;
}

void SpatialDistribution::DistributionArray::indicesNeighborhood9(const unsigned int horizontal, const unsigned int vertical, Indices32& indices) const
{
	ocean_assert(horizontal < horizontalBins_);
	ocean_assert(vertical < verticalBins_);

	ocean_assert(indices.empty());

	for (unsigned int y = max(0, int(vertical) - 1); y < min(vertical + 2u, verticalBins_); ++y)
	{
		for (unsigned int x = max(0, int(horizontal) - 1); x < min(horizontal + 2u, horizontalBins_); ++x)
		{
			const Indices32& localIndices = (*this)(x, y);
			indices.insert(indices.end(), localIndices.begin(), localIndices.end());
		}
	}
}

void SpatialDistribution::DistributionArray::clear()
{
	for (Indices32& indices : indexGroups_)
	{
		indices.clear();
	}

	hasCopiedNeighborhood8_ = false;
}

void SpatialDistribution::idealBins(const unsigned int width, const unsigned int height, const size_t numberBins, unsigned int& horizontalBins, unsigned int& verticalBins, const unsigned int minimalHorizontalBins, const unsigned int minimalVerticalBins)
{
	ocean_assert(width >= 1u && height >= 1u && numberBins >= 1u);
	ocean_assert(minimalHorizontalBins >= 1u && minimalHorizontalBins <= width);
	ocean_assert(minimalVerticalBins >= 1u && minimalVerticalBins <= height);

	// the ideal number of bins (so that the horizontal and vertical sizes of each bin is almost identical) can be determined by:

	// horizontalBins * verticalBins ~ numberBins
	// horizontalBins / verticalBins ~ width / height

	// thus we can determine a good number of vertical bins by:
	// horizontalBins = verticalBins * width / height
	// verticalBins ^ 2 * width / height = numberBins
	// verticalBins = sqrt(numberBins * height / width)

	const Scalar scalarVerticalBins = Numeric::sqrt(Scalar(numberBins) * Scalar(height) / Scalar(width));

	ocean_assert(Numeric::isNotEqualEps(scalarVerticalBins));
	const Scalar scalarHorizontalBins = Scalar(numberBins) / scalarVerticalBins;
	ocean_assert(Numeric::isNotEqualEps(scalarHorizontalBins));

	ocean_assert(Numeric::isWeakEqual(Scalar(width) / scalarHorizontalBins, Scalar(height) / scalarVerticalBins));

	horizontalBins = minmax(minimalHorizontalBins, (unsigned int)(scalarHorizontalBins + Scalar(0.5)), width);
	verticalBins = minmax(minimalVerticalBins, (unsigned int)(scalarVerticalBins + Scalar(0.5)), height);
}

void SpatialDistribution::idealBinsNeighborhood9(const unsigned int width, const unsigned int height, const Scalar distance, unsigned int& horizontalBins, unsigned int& verticalBins, const unsigned int minimalHorizontalBins, const unsigned int minimalVerticalBins, const unsigned int maximalHorizontalBins, const unsigned int maximalVerticalBins)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(distance >= Scalar(1));
	ocean_assert(minimalHorizontalBins >= 1u && minimalHorizontalBins <= maximalHorizontalBins && maximalHorizontalBins <= width);
	ocean_assert(minimalVerticalBins >= 1u && minimalVerticalBins <= maximalVerticalBins && maximalVerticalBins <= height);

	//   +------------+------------+------------+
	//   |            |            |            |
	//   |            |            |            |
	//   |           . -- ~~~ -- . |            |
	//   |       .-~  |            ~-.          |
	//   |      /     |            |   \        |
	//   +-----/------+------------+----\-------+
	//   |    |       |       distance   |      |
	//   |    |       |    x------------>|      |
	//   |    |       |            |     |      |
	//   |     \      |            |    /       |
	//   |      \     |            |   /        |
	//   +-------`-.--+------------+.-'---------+
	//   |           ~- . ___ . -~ |            |
	//   |            |            |            |
	//   |            |            |            |
	//   |            |            |            |
	//   |            |            |            |
	//   +------------+------------+------------+
	//
	//   x---------->| -- distance of one bin element to other elements that
	//                    are guaranteed to be in the 9-neighborhood in this
	//                    distribution array

	const Scalar scalarHorizontalBins = Scalar(width) / distance;
	horizontalBins = minmax(minimalHorizontalBins, (unsigned int)(scalarHorizontalBins + Scalar(0.5)), maximalHorizontalBins);

	const Scalar scalarVerticalBins = Scalar(height) / distance;
	verticalBins = minmax(minimalVerticalBins, (unsigned int)(scalarVerticalBins + Scalar(0.5)), maximalVerticalBins);
}

Indices32 SpatialDistribution::filterAccordingDistance(const ImagePoint* imagePoints, const size_t number, const unsigned int width, const unsigned int height, const Scalar distance)
{
	ocean_assert(imagePoints);
	ocean_assert(distance > 0);

	// the bins size must be larger than the distance to be investigated
	const DistributionArray distributionArray(distributeToArray<40u>(imagePoints, number, Scalar(0), Scalar(0), Scalar(width), Scalar(height), distance));

	Indices32 pointIndices;
	pointIndices.reserve(number);

	const Scalar sqrDistanceThreshold = Numeric::sqr(distance);

	for (unsigned int n = 0u; n < number; ++n)
	{
		const ImagePoint& imagePoint = imagePoints[n];

		const unsigned int xBin = distributionArray.horizontalBin(imagePoint.x());
		const unsigned int yBin = distributionArray.verticalBin(imagePoint.y());

		if (xBin < distributionArray.horizontalBins() && yBin < distributionArray.verticalBins())
		{
			const Indices32 indexVector(distributionArray.indicesNeighborhood9(xBin, yBin));

			bool found = false;

			for (Indices32::const_iterator i = indexVector.begin(); i != indexVector.end(); ++i)
			{
				const unsigned int index = *i;

				ocean_assert(index < number);

				if (index != n)
				{
					const Scalar sqrDistance = imagePoint.sqrDistance(imagePoints[index]);

					if (sqrDistance <= sqrDistanceThreshold)
					{
						found = true;
						break;
					}
				}
			}

			if (!found)
			{
				pointIndices.push_back(n);
			}
		}
	}

	return pointIndices;
}

void SpatialDistribution::filterCandidatePoint(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidatePoints, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar filterDistance, const unsigned int filterSize, Indices32* filteredIndices, ImagePoints* filteredCandidates)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(filteredIndices || filteredCandidates);

	const DistributionArray candidateDistributionArray(distributeToArray<40u>(candidatePoints, numberCandidatePoints, Scalar(0), Scalar(0), Scalar(width), Scalar(height), filterDistance));
	std::vector<unsigned char> useStatements(numberCandidatePoints, 0u);

	Indices32 localIndices;
	localIndices.reserve(50);

	const Scalar sqrDistance = Numeric::sqr(filterDistance);

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const ImagePoint& imagePoint = imagePoints[n];

		const unsigned int xBin = candidateDistributionArray.horizontalBin(imagePoint.x());
		const unsigned int yBin = candidateDistributionArray.verticalBin(imagePoint.y());

		if (xBin < candidateDistributionArray.horizontalBins() && yBin < candidateDistributionArray.verticalBins())
		{
			localIndices.clear();
			candidateDistributionArray.indicesNeighborhood9(xBin, yBin, localIndices);

			// ensure that at most filterSize candidates are used for each image points
			for (unsigned int i = 0u; i < min((unsigned int)localIndices.size(), filterSize); ++i)
			{
				const unsigned int candidateIndex = localIndices[i];

				if (imagePoint.sqrDistance(candidatePoints[candidateIndex]) <= sqrDistance)
				{
					useStatements[candidateIndex] = 1u;
				}
			}
		}
	}

	if (filteredIndices)
	{
		*filteredIndices = Subset::statements2indices<unsigned int, 1u>(useStatements);
	}

	if (filteredCandidates)
	{
		ocean_assert(filteredCandidates->empty());
		filteredCandidates->reserve(useStatements.size());

		for (size_t n = 0; n < useStatements.size(); ++n)
		{
			if (useStatements[n] == 1u)
			{
				filteredCandidates->push_back(candidatePoints[n]);
			}
		}
	}
}

SpatialDistribution::DistributionArray SpatialDistribution::distributeToArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(imagePoints || number == 0);
	ocean_assert(width > 0 && height > 0);

	ocean_assert(horizontalBins >= 1u);
	ocean_assert(verticalBins >= 1u);

	DistributionArray indexArray(left, top, width, height, horizontalBins, verticalBins);

	for (size_t n = 0; n < number; ++n)
	{
		const Vector2& point = *imagePoints;

		const unsigned int horizontal = (unsigned int)(indexArray.horizontalBin(point.x()));
		const unsigned int vertical = (unsigned int)(indexArray.verticalBin(point.y()));

		// discard points with negative bin value or bin value larger than the smallest allowed one (due to unsigned int negative values are discarded as well)
		if (horizontal < indexArray.horizontalBins() && vertical < indexArray.verticalBins())
		{
			indexArray(horizontal, vertical).push_back((unsigned int)n);
		}

		++imagePoints;
	}

	return indexArray;
}

SpatialDistribution::OccupancyArray SpatialDistribution::createOccupancyArray(const ImagePoint* imagePoints, const size_t number, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(imagePoints || number == 0);
	ocean_assert(width > 0 && height > 0);

	ocean_assert(horizontalBins > 0u);
	ocean_assert(verticalBins > 0u);

	ocean_assert(Scalar(horizontalBins) <= width);
	ocean_assert(Scalar(verticalBins) <= width);

	// reserve enough array elements
	OccupancyArray occupancyArray(left, top, width, height, horizontalBins, verticalBins);

	const ImagePoint* endImagePoints = imagePoints + number;
	while (imagePoints != endImagePoints)
	{
		const unsigned int horizontal = (unsigned int)(occupancyArray.horizontalBin(imagePoints->x()));
		const unsigned int vertical = (unsigned int)(occupancyArray.verticalBin(imagePoints->y()));
		imagePoints++;

		// discard points with negative bin value or bin value larger than the smallest allowed one (due to unsigned int negative values are discarded as well)
		if (horizontal < occupancyArray.horizontalBins() && vertical < occupancyArray.verticalBins())
		{
			occupancyArray(horizontal, vertical) = 1u;
		}
	}

	return occupancyArray;
}

SpatialDistribution::DistanceElements SpatialDistribution::sortAccordingDistance(const ImagePoint* imagePoints, const size_t number, const bool minimalDistanceFirst)
{
	ocean_assert(imagePoints);

	if (number == 0)
	{
		return DistanceElements();
	}

	if (number == 1)
	{
		return DistanceElements(1u, DistanceElement(0u, 0xFFFFFFFFu, Numeric::maxValue()));
	}

	DistanceElements distanceElements;
	distanceElements.reserve(number);

	for (unsigned int n = 0; n < number; ++n)
	{
		Scalar minDistance = Numeric::maxValue();
		unsigned int minIndex = 0xFFFFFFFF;

		const Vector2& interestElement = imagePoints[n];

		for (unsigned int i = 0u; i < number; ++i)
		{
			if (i != n)
			{
				const Scalar distance = interestElement.sqrDistance(imagePoints[i]);

				if (distance < minDistance)
				{
					minDistance = distance;
					minIndex = i;
				}
			}
		}

		ocean_assert(minDistance != Numeric::maxValue());
		distanceElements.push_back(DistanceElement(n, minIndex, minDistance));
	}

	if (minimalDistanceFirst)
	{
		std::sort(distanceElements.begin(), distanceElements.end(), DistanceElement::compareLeftSmaller);
	}
	else
	{
		std::sort(distanceElements.begin(), distanceElements.end(), DistanceElement::compareLeftHigher);
	}

	return distanceElements;
}

SpatialDistribution::DistanceElements SpatialDistribution::sortAccordingDistance(const ImagePoint* imagePoints, const size_t number, const unsigned int width, const unsigned int height, const unsigned int bins, const bool minimalDistanceFirst)
{
	ocean_assert(imagePoints);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(bins > 0u);

	if (number == 0)
	{
		return DistanceElements();
	}

	if (number == 1)
	{
		return DistanceElements(1u, DistanceElement(0, 0xFFFFFFFF, Numeric::maxValue()));
	}

	const DistributionArray indexArray(distributeToArray(imagePoints, number, Scalar(0), Scalar(0), Scalar(width), Scalar(height), bins, bins));

	DistanceElements distanceElements;
	distanceElements.reserve(number);

	for (int yBins = 0; yBins < int(bins); ++yBins)
	{
		for (int xBins = 0; xBins < int(bins); ++xBins)
		{
			const Indices32& arrayVector = indexArray[yBins * bins + xBins];

			for (unsigned int n = 0; n < arrayVector.size(); ++n)
			{
				const unsigned int imageIndex = arrayVector[n];
				const Vector2& point = imagePoints[imageIndex];

				Scalar minDistance = Numeric::maxValue();
				unsigned int minIndex = 0xFFFFFFFFu;

				for (int y = max(0, yBins - 1); y <= min(yBins + 1, int(bins) - 1); ++y)
				{
					for (int x = max(0, xBins - 1); x <= min(xBins + 1, int(bins) - 1); ++x)
					{
						const Indices32& neighborArrayVector = indexArray[y * bins + x];

						for (unsigned int i = 0u; i < neighborArrayVector.size(); ++i)
						{
							const unsigned int neighborIndex = neighborArrayVector[i];

							if (neighborIndex != imageIndex)
							{
								const Scalar distance = point.sqrDistance(imagePoints[neighborIndex]);

								if (distance < minDistance)
								{
									minDistance = distance;
									minIndex = neighborIndex;
								}
							}
						}
					}
				}

				distanceElements.push_back(DistanceElement(imageIndex, minIndex, minDistance));
			}
		}
	}

	if (minimalDistanceFirst)
	{
		std::sort(distanceElements.begin(), distanceElements.end(), DistanceElement::compareLeftSmaller);
	}
	else
	{
		std::sort(distanceElements.begin(), distanceElements.end(), DistanceElement::compareLeftHigher);
	}

	return distanceElements;
}

Scalar SpatialDistribution::determineMinimalSqrDistance(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int index, const DistributionArray& distributionElements)
{
	ocean_assert(imagePoints);
	ocean_assert(distributionElements);

	if (numberImagePoints == 0)
	{
		return Numeric::maxValue();
	}

	const Vector2& point = imagePoints[index];

	const int xBin = distributionElements.horizontalBin(point.x());
	const int yBin = distributionElements.verticalBin(point.y());

	ocean_assert(xBin >= 0 && yBin >= 0);
	ocean_assert(xBin < int(distributionElements.horizontalBins()) && yBin < int(distributionElements.verticalBins()));

	Scalar minDistance = Numeric::maxValue();

	for (int y = max(0, yBin - 1); y <= min(yBin + 1, int(distributionElements.verticalBins()) - 1); ++y)
	{
		for (int x = max(0, xBin - 1); x <= min(xBin + 1, int(distributionElements.horizontalBins()) - 1); ++x)
		{
			const Indices32& neighborArrayVector = distributionElements(x, y);

			for (Indices32::const_iterator i = neighborArrayVector.begin(); i != neighborArrayVector.end(); ++i)
			{
				if (*i != index)
				{
					const Scalar distance = point.sqrDistance(imagePoints[*i]);

					if (distance < minDistance)
						minDistance = distance;
				}
			}
		}
	}

	return minDistance;
}

void SpatialDistribution::determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances)
{
	ocean_assert(imagePoints && sqrDistances);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(bins > 0u);

	if (numberImagePoints == 0)
	{
		return;
	}

	const DistributionArray indexArray(distributeToArray(imagePoints, numberImagePoints, Scalar(0), Scalar(0), Scalar(width), Scalar(height), bins, bins));

	const Scalar horizontalFactor = Scalar(bins) / Scalar(width);
	const Scalar verticalFactor = Scalar(bins) / Scalar(height);

	for (unsigned int n = 0u; n < numberImagePoints; ++n)
	{
		const Vector2& point = imagePoints[n];

		const int xBins = (unsigned int)(point.x() * horizontalFactor);
		const int yBins = (unsigned int)(point.y() * verticalFactor);

		ocean_assert(xBins >= 0 && yBins >= 0);
		ocean_assert(xBins < int(bins) && yBins < int(bins));

		Scalar minDistance = Numeric::maxValue();

		for (int y = max(0, yBins - 1); y <= min(yBins + 1, int(bins) - 1); ++y)
		{
			for (int x = max(0, xBins - 1); x <= min(xBins + 1, int(bins) - 1); ++x)
			{
				const Indices32& neighborArrayVector = indexArray[y * bins + x];

				for (Indices32::const_iterator i = neighborArrayVector.begin(); i != neighborArrayVector.end(); ++i)
				{
					if (*i != n)
					{
						const Scalar distance = point.sqrDistance(imagePoints[*i]);

						if (distance < minDistance)
							minDistance = distance;
					}
				}
			}
		}

		sqrDistances[n] = minDistance;
	}
}

void SpatialDistribution::determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidates, const size_t numberCandidates, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances)
{
	ocean_assert(imagePoints || numberImagePoints == 0);
	ocean_assert(candidates || numberCandidates == 0);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(bins > 0u);

	if (numberImagePoints == 0 || numberCandidates == 0)
	{
		return;
	}

	ocean_assert(sqrDistances);

	const DistributionArray indexArray(distributeToArray(candidates, numberCandidates, Scalar(0), Scalar(0), Scalar(width), Scalar(height), bins, bins));
	determineMinimalSqrDistances(imagePoints, numberImagePoints, candidates, numberCandidates, indexArray, sqrDistances);
}

void SpatialDistribution::determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const ImagePoint* candidates, const size_t numberCandidates, const DistributionArray& distributionCandidates, Scalar* sqrDistances, unsigned int* candidateIndices)
{
	ocean_assert(imagePoints || numberImagePoints == 0);
	ocean_assert(candidates || numberCandidates == 0);

	if (numberImagePoints == 0 || numberCandidates == 0)
	{
		return;
	}

	ocean_assert(distributionCandidates && sqrDistances);

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& point = imagePoints[n];

		const int xBins = distributionCandidates.horizontalBin(point.x());
		const int yBins = distributionCandidates.verticalBin(point.y());

		Scalar minDistance = Numeric::maxValue();
		unsigned int minDistanceIndex = 0xFFFFFFFFu;

		// we look for the nearest point within the 9-neighborhood
		for (int y = max(0, yBins - 1); y <= min(yBins + 1, int(distributionCandidates.verticalBins()) - 1); ++y)
		{
			for (int x = max(0, xBins - 1); x <= min(xBins + 1, int(distributionCandidates.horizontalBins()) - 1); ++x)
			{
				const Indices32& candidatesIndices = distributionCandidates(x, y);

				for (unsigned int i = 0u; i < candidatesIndices.size(); ++i)
				{
					const unsigned int candidateIndex = candidatesIndices[i];

					if (candidateIndex < numberCandidates)
					{
						const Scalar distance = point.sqrDistance(candidates[candidateIndex]);

						if (distance < minDistance)
						{
							minDistance = distance;
							minDistanceIndex = candidateIndex;
						}
					}
				}
			}
		}

		sqrDistances[n] = minDistance;

		if (candidateIndices)
		{
			candidateIndices[n] = minDistanceIndex;
		}
	}
}

void SpatialDistribution::determineMinimalSqrDistances(const ImagePoint* imagePoints, const size_t numberImagePoints, const unsigned int* interestIndices, const size_t numberInterestIndices, const unsigned int width, const unsigned int height, const unsigned int bins, Scalar* sqrDistances)
{
	ocean_assert(imagePoints && interestIndices && sqrDistances);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(bins > 0u);

	if (numberImagePoints == 0 || numberInterestIndices == 0)
	{
		return;
	}

	const DistributionArray indexArray(distributeToArray(imagePoints, numberImagePoints, Scalar(0), Scalar(0), Scalar(width), Scalar(height), bins, bins));

	const Scalar horizontalFactor = Scalar(bins) / Scalar(width);
	const Scalar verticalFactor = Scalar(bins) / Scalar(height);

	for (unsigned int n = 0; n < numberInterestIndices; ++n)
	{
		const unsigned int interestIndex = interestIndices[n];
		ocean_assert(interestIndex < numberImagePoints);

		const Vector2& point = imagePoints[interestIndex];

		const int xBins = (unsigned int)(point.x() * horizontalFactor);
		const int yBins = (unsigned int)(point.y() * verticalFactor);

		ocean_assert(xBins >= 0 && yBins >= 0);
		ocean_assert(xBins < int(bins) && yBins < int(bins));

		Scalar minDistance = Numeric::maxValue();

		for (int y = max(0, yBins - 1); y <= min(yBins + 1, int(bins) - 1); ++y)
		{
			for (int x = max(0, xBins - 1); x <= min(xBins + 1, int(bins) - 1); ++x)
			{
				const Indices32& neighborArrayVector = indexArray[y * bins + x];

				for (unsigned int i = 0u; i < neighborArrayVector.size(); ++i)
				{
					const unsigned int neighborIndex = neighborArrayVector[i];

					if (neighborIndex != interestIndex)
					{
						const Scalar distance = point.sqrDistance(imagePoints[neighborIndex]);

						if (distance < minDistance)
							minDistance = distance;
					}
				}
			}
		}

		sqrDistances[n] = minDistance;
	}
}

Indices32 SpatialDistribution::determineNeighbors(const ImagePoint& point, const ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar radius, const DistributionArray& distributionCandidatePoints)
{
	ocean_assert(candidatePoints && distributionCandidatePoints);

	const int horizontalBin = distributionCandidatePoints.clampedHorizontalBin(point.x());
	const int verticalBin = distributionCandidatePoints.clampedVerticalBin(point.y());

	const Scalar sqrRadius = Numeric::sqr(radius);

	const Indices32 indices(distributionCandidatePoints.indicesNeighborhood9(horizontalBin, verticalBin));

	Indices32 result;
	result.reserve(indices.size());

	for (Indices32::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		const unsigned int neighborIndex = *i;
		ocean_assert_and_suppress_unused(neighborIndex < numberCandidatePoints, numberCandidatePoints);

		if (point.sqrDistance(candidatePoints[neighborIndex]) <= sqrRadius)
		{
			result.push_back(neighborIndex);
		}
	}

	return result;
}

Index32 SpatialDistribution::determineNearestNeighbor(const ImagePoint& interestPoint, const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar radius, const DistributionArray& distributionImagePoints, Scalar* sqrDistance)
{
	if (numberImagePoints == 0)
	{
		return Index32(-1);
	}

	ocean_assert(imagePoints && distributionImagePoints);

	const int horizontalBin = distributionImagePoints.clampedHorizontalBin(interestPoint.x());
	const int verticalBin = distributionImagePoints.clampedVerticalBin(interestPoint.y());

	const Indices32 indices(distributionImagePoints.indicesNeighborhood9(horizontalBin, verticalBin));

	Index32 bestIndex = Index32(-1);
	Scalar bestSqrDistance = Numeric::sqr(radius) + Numeric::eps();

	for (Indices32::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		const unsigned int neighborIndex = *i;
		ocean_assert(neighborIndex < numberImagePoints);

		const Scalar localSqrDistance = interestPoint.sqrDistance(imagePoints[neighborIndex]);

		if (localSqrDistance < bestSqrDistance)
		{
			bestIndex = neighborIndex;
			bestSqrDistance = localSqrDistance;
		}
	}

	if (sqrDistance && bestIndex != (unsigned int)(-1))
	{
		*sqrDistance = bestSqrDistance;
	}

	return bestIndex;
}

ImagePoints SpatialDistribution::distributeAndFilter(const ImagePoint* imagePoints, const size_t numberImagePoints, const Scalar left, const Scalar top, const Scalar width, const Scalar height, const unsigned int horizontalBins, const unsigned int verticalBins, const size_t size)
{
	ocean_assert(imagePoints || numberImagePoints == 0);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(horizontalBins >= 1u && Scalar(horizontalBins) <= width);
	ocean_assert(verticalBins >= 1u && Scalar(verticalBins) <= height);

	ocean_assert(size <= numberImagePoints);

	if (numberImagePoints == 0 || size == 0)
	{
		return ImagePoints();
	}

	// if we are looking for more (or equal) points than we actually have we simply return all
	if (size >= numberImagePoints)
	{
		ImagePoints result(numberImagePoints);

		for (size_t n = 0; n < numberImagePoints; ++n)
		{
			result[n] = imagePoints[n];
		}

		return result;
	}

	const DistributionArray distribution(Geometry::SpatialDistribution::distributeToArray(imagePoints, numberImagePoints, left, top, width, height, horizontalBins, verticalBins));

	ImagePoints results;
	results.reserve(size);

	size_t iteration = 0;

	while (results.size() < size)
	{
		// take the first feature from each bin in the first iteration, take the second feature point from each bin in the second iteration, ...
		for (unsigned int n = 0u; n < distribution.bins() && results.size() < size; ++n)
		{
			const Indices32& indices = distribution[n];

			if (indices.size() > iteration)
			{
				ocean_assert(indices.front() < numberImagePoints);
				results.push_back(imagePoints[indices[iteration]]);
			}
		}

		iteration++;
	}

	return results;
}

}

}
