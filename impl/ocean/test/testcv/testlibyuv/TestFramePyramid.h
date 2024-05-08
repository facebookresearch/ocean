/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_PYRAMID_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_PYRAMID_H

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
 * This class tests/benchmarks the performance of frame pyramids.
 * @ingroup testcvlibyuv
 */
class OCEAN_TEST_CV_LIBYUV_EXPORT TestFramePyramid
{
	public:

		/**
		 * Benchmarks the performance of libyuv agains Ocean for frame pyramid functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:
	
		/**
		 * Benchmarks the creation of a frame pyramid.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testCreationFramePyramid(const double testDuration);

		/**
		 * Benchmarks the creation of a frame pyramid.
		 * @param width The width of the finest pyramid layer in pixel, with range [1, infinity)
		 * @param height The height of the finest pyramid layer in pixel, with range [1, infinity)
		 * @param channels The number of channels both frames have, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testCreationFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_PYRAMID_H
