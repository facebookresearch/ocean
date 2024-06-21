/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_CONVERTER_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_CONVERTER_H

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
 * This class tests the performance of the frame converter function of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFrameConverter
{
	public:

		/**
		 * Benchmarks frame converter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the performance of cast of a frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testCast(const double testDuration);
	
		/**
		 * Benchmarks the performance of cast of a frame.
		 * @param width The width of the frame to cast, with range [1, infinity)
		 * @param height The height of the frame to cast, with range [1, infinity)
		 * @param channels The nmber of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testCast(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_CONVERTER_H
