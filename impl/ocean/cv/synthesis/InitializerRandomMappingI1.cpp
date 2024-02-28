// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/InitializerRandomMappingI1.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

void InitializerRandomMappingI1::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	const unsigned int layerWidth = layerI_.width();
	const unsigned int layerHeight = layerI_.height();

	ocean_assert(firstRow + numberRows <= layerHeight);
	ocean_assert(firstColumn + numberColumns <= layerWidth);

	RandomGenerator generator(randomGenerator_);

	const uint8_t* const mask = layerI_.legacyMask().constdata<uint8_t>();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		PixelPosition* mapping = layerI_.mapping()() + y * layerWidth + firstColumn;
		const uint8_t* maskPtr = mask + y * layerWidth + firstColumn;
		const uint8_t* const maskPtrEnd = maskPtr + numberColumns;

		unsigned int randomX, randomY;

		while (maskPtr != maskPtrEnd)
		{
			ocean_assert(maskPtr < maskPtrEnd);

			if (*maskPtr != 0xFF)
			{
				do
				{
					randomX = RandomI::random(generator, layerWidth - 1u);
					randomY = RandomI::random(generator, layerHeight - 1u);
				}
				while (mask[randomY * layerWidth + randomX] != 0xFF);

				*mapping = PixelPosition(randomX, randomY);
			}

			++maskPtr;
			++mapping;
		}
	}
}

}

}

}
