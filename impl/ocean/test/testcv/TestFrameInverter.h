/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_FRAME_INVERTER_H
#define OCEAN_TEST_TESTCV_TEST_FRAME_INVERTER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameInverter class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInverter
{
	public:

		/**
		 * Starts all tests of the FrameInverter class.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the invert function for images with 8 bit per channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testInvert8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);
};

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_FRAME_INVERTER_H
