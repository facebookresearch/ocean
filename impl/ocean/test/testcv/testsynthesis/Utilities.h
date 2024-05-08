/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_UTILITIES_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * This class implements some utilities functions.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT Utilities
{
	public:

		/**
		 * Returns a randomized binary mask for inpainting, the mask may contain random padding at the end of each row.
		 * @param width The width of the mask, in pixel, with range [1, infinity)
		 * @param height The height of the mask, in pixel, with range [1, infinity)
		 * @param maskValue The mask value to be used, with range [0, 255]
		 * @param randomGenerator The random generator to be used
		 * @return The resulting mask
		 */
		static Frame randomizedInpaintingMask(const unsigned int width, const unsigned int height, const uint8_t maskValue, RandomGenerator& randomGenerator);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_UTILITIES_H
