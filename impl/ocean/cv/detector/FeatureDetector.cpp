/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/FeatureDetector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/geometry/SpatialDistribution.h"

#include <algorithm>

namespace Ocean
{

namespace CV
{

namespace Detector
{

Vectors2 FeatureDetector::filterStrongHarrisPoints(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vectors2& positions, const unsigned int maximalPoints, const Scalar minSqrDistance, const int harrisThreshold, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 5u && height >= 5u);

	if (positions.empty())
	{
		return Vectors2();
	}

	const CV::PixelPositions pixelPositions(CV::PixelPosition::vectors2pixelPositions(positions));

	std::vector<int> votes(positions.size());
	CV::Detector::HarrisCornerDetector::harrisVotes(yFrame, width, height, yFramePaddingElements, pixelPositions.data(), pixelPositions.size(), votes.data(), worker);

	IntensityVectors2 intensities;
	intensities.reserve(positions.size());

	for (unsigned int n = 0u; n < positions.size(); ++n)
	{
		intensities.push_back(IntensityVector2(positions[n], votes[n]));
	}

	std::sort(intensities.begin(), intensities.end());

	Vectors2 strongestPositions;
	strongestPositions.reserve(maximalPoints);

	for (IntensityVectors2::const_iterator i = intensities.begin(); i != intensities.end() && strongestPositions.size() < maximalPoints; ++i)
	{
		if (i->intensity() >= harrisThreshold)
		{
			bool neighborhood = false;

			for (Vectors2::const_iterator iP = strongestPositions.begin(); iP != strongestPositions.end(); ++iP)
			{
				if (i->sqrDistance(*iP) < minSqrDistance)
				{
					neighborhood = true;
					break;
				}
			}

			if (!neighborhood)
			{
				strongestPositions.push_back(*i);
			}
		}
	}

	return strongestPositions;
}

Vectors2 FeatureDetector::determineHarrisPoints(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const SubRegion& subRegion, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker, std::vector<int>* strengths)
{
	ocean_assert(yFrame);
	ocean_assert(width >= 7u && height >= 7u);

	// Calculate bounding box around all regions:
	Box2 boundingBox = subRegion.boundingBox().isValid() ? subRegion.boundingBox() : Box2();
	if (!boundingBox.isValid())
	{
		boundingBox = Box2(0, 0, Scalar(width), Scalar(height));
	}

	// Calculate clip window by intersecting bounding box with image borders:
	unsigned int windowLeft, windowTop, windowWidth, windowHeight;
	if (!boundingBox.box2integer(width, height, windowLeft, windowTop, windowWidth, windowHeight))
	{
		return Vectors2();
	}

	ocean_assert(windowWidth >= 1u && windowWidth <= width);
	ocean_assert(windowHeight >= 1u && windowHeight <= height);

	Detector::HarrisCorners corners;
	if (!Detector::HarrisCornerDetector::detectCorners(yFrame, width, height, yFramePaddingElements, windowLeft, windowTop, windowWidth, windowHeight, strength, true, corners, true, worker))
		return Vectors2();

	// If first run went bad, we try again with lowered expectations:
	if (corners.size() < 50)
	{
		corners.clear();
		if (!Detector::HarrisCornerDetector::detectCorners(yFrame, width, height, yFramePaddingElements, windowLeft, windowTop, windowWidth, windowHeight, strength / 2u, true, corners, true, worker))
		{
			return Vectors2();
		}
	}

	// If second run went bad, we try once more with even lower expectations:
	if (corners.size() < 20)
	{
		corners.clear();
		if (!Detector::HarrisCornerDetector::detectCorners(yFrame, width, height, yFramePaddingElements, windowLeft, windowTop, windowWidth, windowHeight, strength / 4u, true, corners, true, worker))
		{
			return Vectors2();
		}
	}

	if (corners.empty())
	{
		return Vectors2();
	}

	// Restrict corners to those lying inside sub regions:
	Detector::HarrisCorners cornersSubRegion;

	// if no sub-area filter is provided, use all corners
	if (subRegion.isEmpty())
	{
		cornersSubRegion = std::move(corners);
	}
	else
	{
		cornersSubRegion.reserve(corners.size());

		for (Detector::HarrisCorners::const_iterator i = corners.begin(); i != corners.end(); ++i)
		{
			if (subRegion.isInside(i->observation()))
			{
				cornersSubRegion.push_back(*i);
			}
		}
	}

	// sort corners with decreasing strength
	std::sort(cornersSubRegion.begin(), cornersSubRegion.end());

	// keep only the strongest corner per bin
	if (!cornersSubRegion.empty() && horizontalBins != 0u && verticalBins != 0u)
	{
		cornersSubRegion = Geometry::SpatialDistribution::distributeAndFilter<Detector::HarrisCorner, Detector::HarrisCorner::corner2imagePoint>(cornersSubRegion.data(), cornersSubRegion.size(), Scalar(windowLeft), Scalar(windowTop), Scalar(windowWidth), Scalar(windowHeight), horizontalBins, verticalBins);
	}

	// check whether the caller interested also in the strength parameters
	if (strengths)
	{
		Vectors2 points;
		points.reserve(cornersSubRegion.size());

		ocean_assert(strengths->empty());
		strengths->clear();
		strengths->reserve(cornersSubRegion.size());

		for (Detector::HarrisCorners::const_iterator i = cornersSubRegion.begin(); i != cornersSubRegion.end(); ++i)
		{
			points.push_back(i->observation());

			ocean_assert(Scalar(NumericT<int>::minValue()) <= i->strength() && i->strength() <= Scalar(NumericT<int>::maxValue()));
			strengths->push_back(int(i->strength()));
		}

		return points;
	}

	return Detector::HarrisCorner::corners2imagePoints(cornersSubRegion);
}

}

}

}
