/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/OccupancyArray.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

OccupancyArray::OccupancyArray(const Scalar left, const Scalar top, const unsigned int width, const unsigned int height, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int neighborhoodSize, const float minCoverageThreshold) :
	Array(left, top, Scalar(width), Scalar(height), horizontalBins, verticalBins)
{
	ocean_assert(neighborhoodSize >= 1u && neighborhoodSize % 2u == 1u);

	bins_.resize(bins(), 0u);

	ocean_assert(minCoverageThreshold >= 0.0f && minCoverageThreshold <= 1.0);

	coverageThreshold_ = minCoverageThreshold;
	neighborhoodRadius_ = neighborhoodSize / 2u;

	ocean_assert(isValid());
}

void OccupancyArray::addPoint(const unsigned int xBin, const unsigned int yBin)
{
	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	bins_[yBin * horizontalBins_ + xBin]++;

	++numberPoints_;
}

bool OccupancyArray::addPointIfEmpty(const unsigned int xBin, const unsigned int yBin)
{
	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	const unsigned int xStart = (unsigned int)(std::max(0, int(xBin) - int(neighborhoodRadius_)));
	const unsigned int xEnd = std::min(horizontalBins_, xBin + neighborhoodRadius_ + 1u);

	const unsigned int yStart = (unsigned int)(std::max(0, int(yBin) - int(neighborhoodRadius_)));
	const unsigned int yEnd = std::min(verticalBins_, yBin + neighborhoodRadius_ + 1u);

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			const uint32_t& occupancyBin = bins_[y * horizontalBins_ + x];

			if (occupancyBin != 0u)
			{
				return false;
			}
		}
	}

	bins_[yBin * horizontalBins_ + xBin]++;

	++numberPoints_;

	return true;
}

bool OccupancyArray::addPointIfWithinLimit(const unsigned int xBin, const unsigned int yBin, const unsigned int maxPoints)
{
	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	const unsigned int xStart = (unsigned int)(std::max(0, int(xBin) - int(neighborhoodRadius_)));
	const unsigned int xEnd = std::min(horizontalBins_, xBin + neighborhoodRadius_ + 1u);

	const unsigned int yStart = (unsigned int)(std::max(0, int(yBin) - int(neighborhoodRadius_)));
	const unsigned int yEnd = std::min(verticalBins_, yBin + neighborhoodRadius_ + 1u);

	unsigned int points = 0u;

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			const uint32_t& occupancyBin = bins_[y * horizontalBins_ + x];

			points += occupancyBin;
		}
	}

	if (points >= maxPoints)
	{
		return false;
	}

	bins_[yBin * horizontalBins_ + xBin]++;

	++numberPoints_;

	return true;
}

bool OccupancyArray::isEmpty(const unsigned int xBin, const unsigned int yBin) const
{
	ocean_assert(xBin < horizontalBins());
	ocean_assert(yBin < verticalBins());

	const unsigned int xStart = (unsigned int)(std::max(0, int(xBin) - int(neighborhoodRadius_)));
	const unsigned int xEnd = std::min(horizontalBins_, xBin + neighborhoodRadius_ + 1u);

	const unsigned int yStart = (unsigned int)(std::max(0, int(yBin) - int(neighborhoodRadius_)));
	const unsigned int yEnd = std::min(verticalBins_, yBin + neighborhoodRadius_ + 1u);

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			const uint32_t& occupancyBin = bins_[y * horizontalBins_ + x];

			if (occupancyBin != 0u)
			{
				return false;
			}
		}
	}

	return true;
}

}

}

}
