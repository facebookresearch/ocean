/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_UTILITIES_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements utility functions for the detector test library.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT Utilities
{
	public:

		/**
		 * Creates a randomized grayscale (8 bit, one channel) frame with some feature points.
		 * @param width The width of the resulting frame in pixel, with range [1, infinity)
		 * @param height The height of the resulting frame in pixel, with range [1, infinity)
		 * @param featurePointPercentage The percentage of feature points to create, in relation to the image resolution, with range [0, 100]
		 * @param randomGenerator Optional explicit random generator to be used, nullptr to use any
		 * @return The resulting frame
		 */
		static Frame createRandomFrameWithFeatures(const unsigned int width, const unsigned int height, const unsigned int featurePointPercentage = 2u, RandomGenerator* randomGenerator = nullptr);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HARRIS_DETECTOR_H
