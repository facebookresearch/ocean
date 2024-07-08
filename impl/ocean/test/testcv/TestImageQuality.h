/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_IMAGE_QUALITY_H
#define OCEAN_TEST_TESTCV_TEST_IMAGE_QUALITY_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the ImageQuality class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestImageQuality
{
	public:

		/**
		 * Starts all tests of the ImageQuality class.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Applies a stress test for structural similarity calculation ensuring that the function does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testStructuralSimilarityStressTest(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test for multi-scale structural similarity calculation ensuring that the function does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testMultiScaleStructuralSimilarityStressTest(const double testDuration, Worker& worker);
};

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_IMAGE_QUALITY_H
