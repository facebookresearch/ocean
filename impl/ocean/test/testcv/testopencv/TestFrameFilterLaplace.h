/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_FILTER_LAPLACE_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_FILTER_LAPLACE_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include <opencv2/core.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class benchmarks the performance of the Laplace filter functionality of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFrameFilterLaplace
{
	public:

		/**
		 * Benchmarks Laplace filter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the performance of the 'unsigned char' to 'short' filter function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test1Channel8BitWith16BitResponse(const double testDuration);

		/**
		 * Benchmarks the performance of the 'unsigned char' to 'short' filter function.
		 * @param width The width of the source (and target) frame in pixel, with range [3, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test1Channel8BitWith16BitResponse(const unsigned int width, const unsigned int height, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_FILTER_LAPLACE_H
