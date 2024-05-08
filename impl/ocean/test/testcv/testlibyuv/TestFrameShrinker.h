/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_SHRINKER_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_SHRINKER_H

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

/**
 * This class tests/benchmarks the performance of frame shrinking.
 * @ingroup testcvlibyuv
 */
class OCEAN_TEST_CV_LIBYUV_EXPORT TestFrameShrinker
{
	public:

		/**
		 * Benchmarks the performance of libyuv agains Ocean for frame shrinking functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:
	
		/**
		 * Benchmarks the shrinking of frames by a factor of two.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testDivideByTwo(const double testDuration);

		/**
		 * Benchmarks the shrinking of a specific source frame by a factor of two
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels both frames have, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testDivideByTwo(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_SHRINKER_H
