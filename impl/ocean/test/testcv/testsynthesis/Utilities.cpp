/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

Frame Utilities::randomizedInpaintingMask(const unsigned int width, const unsigned int height, const uint8_t maskValue, RandomGenerator& randomGenerator)
{
	ocean_assert(width >= 1u && height >= 1u);

	while (true)
	{
		Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);

		const unsigned int numberRectangles = RandomI::random(randomGenerator, 2u);
		const unsigned int numberEllipses = RandomI::random(randomGenerator, 2u);

		for (unsigned int n = 0u; n < numberRectangles; ++n)
		{
			const unsigned int xSize = RandomI::random(randomGenerator, 2u, std::max(2u, width / 2u));
			const unsigned int ySize = RandomI::random(randomGenerator, 2u, std::max(2u, height / 2u));

			const int left = RandomI::random(randomGenerator, -int(xSize / 2u), int(width + xSize / 2u));
			const int top = RandomI::random(randomGenerator, -int(ySize / 2u), int(height + ySize / 2u));

			CV::Canvas::rectangle(mask, left, top, xSize, ySize, &maskValue);
		}

		for (unsigned int n = 0u; n < numberEllipses; ++n)
		{
			const unsigned int xSize = RandomI::random(randomGenerator, 3u, std::max(3u, width / 2u)) | 0x01u;
			const unsigned int ySize = RandomI::random(randomGenerator, 3u, std::max(3u, height / 2u)) | 0x01u;

			const unsigned int xCenter = RandomI::random(randomGenerator, width - 1u);
			const unsigned int yCenter = RandomI::random(randomGenerator, height - 1u);

			CV::Canvas::ellipse(mask, CV::PixelPosition(xCenter, yCenter), xSize, ySize, &maskValue);
		}

		if (width >= 10u && height >= 10u)
		{
			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				// in 50% of the cases we add a blank border

				const unsigned int left = RandomI::random(randomGenerator, 1u, width / 4u);
				const unsigned int right = RandomI::random(randomGenerator, 1u, width / 4u);

				const unsigned int top = RandomI::random(randomGenerator, 1u, height / 4u);
				const unsigned int bottom = RandomI::random(randomGenerator, 1u, height / 4u);

				const uint8_t blankValue = 0xFFu - maskValue;

				mask.subFrame(0u, 0u, mask.width(), top).setValue(blankValue); // top
				mask.subFrame(0u, 0u, left, mask.height()).setValue(blankValue); // left
				mask.subFrame(mask.width() - right, 0u, right, mask.height()).setValue(blankValue); // right
				mask.subFrame(0u, mask.height() - bottom, mask.width(), bottom).setValue(blankValue); // bottom
			}
		}

		// let's ensure that we have at least one non-mask pixel

		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			for (unsigned int x = 0u; x < mask.width(); ++x)
			{
				if (mask.constpixel<uint8_t>(x, y)[0] != maskValue)
				{
					return mask;
				}
			}
		}
	}
}

}

}

}

}
