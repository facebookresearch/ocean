// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	while (true)
	{
		unsigned int maskPaddingElements = 0u;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			maskPaddingElements = RandomI::random(randomGenerator, 1u, 100u);
		}

		Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, maskPaddingElements, &randomGenerator);

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

Frame Utilities::randomizedInpaintingMaskWithoutPadding(const unsigned int width, const unsigned int height, const uint8_t maskValue, RandomGenerator& randomGenerator)
{
	Frame mask = randomizedInpaintingMask(width, height, maskValue, randomGenerator);
	mask.makeContinuous();

	return mask;
}

}

}

}

}
