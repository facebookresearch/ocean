/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_INTERPOLATOR_BILINEAR_H

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
 * This class tests/benchmarks the performance of frame resizing/rescalilng.
 * @ingroup testcvlibyuv
 */
class OCEAN_TEST_CV_LIBYUV_EXPORT TestFrameInterpolatorBilinear
{
	public:

		/**
		 * Benchmarks the performance of libyuv agains Ocean for frame resize functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the resizing of frames.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testResize(const double testDuration);

		/**
		 * Benchmarks the resizing of a specific source frame to a specific target frame.
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels both frames have, with range [1, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
