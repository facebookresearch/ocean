/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

	const uint8_t* const maskData = layerI_.mask().constdata<uint8_t>();

	const unsigned int maskStrideElements = layerI_.mask().strideElements();

	RandomGenerator generator(randomGenerator_);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		PixelPosition* mappingRow = layerI_.mapping().row(y);

		const uint8_t* maskRow = layerI_.mask().constrow<uint8_t>(y);

		unsigned int randomX, randomY;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskRow[x] != 0xFFu)
			{
				do
				{
					randomX = RandomI::random(generator, layerWidth - 1u);
					randomY = RandomI::random(generator, layerHeight - 1u);
				}
				while (maskData[randomY * maskStrideElements + randomX] != 0xFFu);

				mappingRow[x] = PixelPosition(randomX, randomY);
			}
		}
	}
}

}

}

}
