/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/ContourMapping.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

unsigned int ContourMapping::neighborCostDistance(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex)
{
	ocean_assert(sourceIndex < mapping.size());
	ocean_assert(targetIndex < mapping.size());

	const unsigned int sourceIndexMinus = (unsigned int)modulo(int(sourceIndex) - 1, int(mapping.size()));

	const unsigned int targetIndexPlus = mapping[sourceIndexMinus];
	const unsigned int costPlus = targetIndexPlus == (unsigned int)(-1) ? 0u : abs(int(ringDistance(int(targetIndex), int(targetIndexPlus), int(mapping.size()))) - 1);

	const unsigned int sourceIndexPlus = (unsigned int)modulo(int(sourceIndex) + 1, int(mapping.size()));

	const unsigned int targetIndexMinus = mapping[sourceIndexPlus];
	const unsigned int costMinus = targetIndexMinus == (unsigned int)(-1) ? 0u : abs(int(ringDistance(int(targetIndex), int(targetIndexMinus), int(mapping.size()))) - 1);

	return costPlus + costMinus;
}

unsigned int ContourMapping::neighborCostDistanceMinus(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex)
{
	ocean_assert(sourceIndex < mapping.size());
	ocean_assert(targetIndex < mapping.size());

	for (size_t n = 1; n < mapping.size(); ++n)
	{
		const unsigned int sourceIndexMinus = (unsigned int)modulo(int(sourceIndex) - int(n), int(mapping.size()));
		const unsigned int targetIndexMinus = mapping[sourceIndexMinus];

		if (targetIndexMinus != (unsigned int)(-1))
		{
			const int distance = distanceInContour(targetIndexMinus, targetIndex, (unsigned int)mapping.size());
			return abs(abs(distance) - int(n));
		}
	}

	return 0u;
}

unsigned int ContourMapping::neighborCostDistancePlus(const Indices32& mapping, const unsigned int sourceIndex, const unsigned int targetIndex)
{
	ocean_assert(sourceIndex < mapping.size());
	ocean_assert(targetIndex < mapping.size());

	for (size_t n = 1; n < mapping.size(); ++n)
	{
		const unsigned int sourceIndexPlus = (unsigned int)modulo(int(sourceIndex) + int(n), int(mapping.size()));
		const unsigned int targetIndexPlus = mapping[sourceIndexPlus];

		if (targetIndexPlus != (unsigned int)(-1))
		{
			const int distance = distanceInContour(targetIndex, targetIndexPlus, (unsigned int)mapping.size());
			return abs(abs(distance) - int(n));
		}
	}

	return 0u;
}

int ContourMapping::distanceInContour(const unsigned int highIndex, const unsigned int lowIndex, const unsigned int size)
{
	const unsigned int distanceInRing = ringDistance(lowIndex, highIndex, size);

	if (highIndex >= lowIndex)
	{
		if (highIndex - lowIndex == distanceInRing)
		{
			return int(distanceInRing);
		}

		return -int(distanceInRing);
	}
	else // highIndex < lowIndex
	{
		if (lowIndex - highIndex == distanceInRing)
		{
			return -int(distanceInRing);
		}

		return distanceInRing;
	}
}

bool ContourMapping::isLineOutsideMask(const uint8_t* mask, const uint8_t* outline, const unsigned int width, const unsigned int height, const unsigned int maskStrideElements, const unsigned int outlineStrideElements, const PixelPosition& start, const PixelPosition& stop)
{
	ocean_assert(mask != nullptr && outline != nullptr);
	ocean_assert_and_suppress_unused(width != 0u && height != 0u, height);
	ocean_assert_and_suppress_unused(width <= maskStrideElements, width);
	ocean_assert_and_suppress_unused(width <= outlineStrideElements, width);

	int x = int(start.x());
	int y = int(start.y());

	int xStop = int(stop.x());
	int yStop = int(stop.y());

	CV::Bresenham bresenham(x, y, xStop, yStop);
	bresenham.findNext(x, y);

	bool validLine = false;

	while (x != xStop || y != yStop)
	{
		ocean_assert(x >= 0 && x < int(width));
		ocean_assert(y >= 0 && y < int(height));

		if (mask[y * maskStrideElements + x] == 0xFF)
		{
			if (outline[y * outlineStrideElements + x] != 0x00)
			{
				return true;
			}
		}
		else
		{
			validLine = true;
		}

		bresenham.findNext(x, y);
	}

	return !validLine;
}

}

}

}
