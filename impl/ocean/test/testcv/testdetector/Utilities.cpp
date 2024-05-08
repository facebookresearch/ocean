/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/Utilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

Frame Utilities::createRandomFrameWithFeatures(const unsigned int width, const unsigned int height, const unsigned int featurePointPercentage, RandomGenerator* randomGenerator)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(featurePointPercentage <= 100u);

	RandomGenerator localRandomGenerator(randomGenerator);

	Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &localRandomGenerator);

	// we apply a strong blur filter
	CV::FrameFilterGaussian::filter(yFrame, 11u);

	const unsigned int featurePoints = yFrame.pixels() * featurePointPercentage / 100u;

	for (unsigned int n = 0u; n < featurePoints; ++n)
	{
		const unsigned int x = RandomI::random(localRandomGenerator, 0u, width - 1u);
		const unsigned int y = RandomI::random(localRandomGenerator, 0u, height - 1u);

		const uint8_t value = RandomI::random(localRandomGenerator, 1u) == 0u ? 0x00 : 0xFF;

		yFrame.pixel<uint8_t>(x, y)[0] = value;
	}

	return yFrame;
}

}

}

}

}
