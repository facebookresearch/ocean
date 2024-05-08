/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BICUBIC_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BICUBIC_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a bicubic frame interpolator test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorBicubic
{
	public:

		/**
		 * Tests all bicubic interpolation filter functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	private:

		/**
		 * Tests the bicubic resize function for 8 bit grayscale frames.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sizeFactor Ratio between target frame size and source frame size
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels
		 */
		template <unsigned int tChannels>
		static bool testResize8BitPerChannel(const unsigned int width, const unsigned int height, const float sizeFactor, const double testDuration, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BICUBIC_H
