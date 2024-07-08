/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_MIN_MAX_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_MIN_MAX_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class benchmarks the performance of the minmax functionality of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFrameMinMax
{
	public:

		/**
		 * Benchmarks min-max functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the performance of function allowing to determine the location of a minimum value within a 1-channel frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testMinLocation(const double testDuration);

		/**
		 * Benchmarks the performance of function allowing to determine the location of a maximum value within a 1-channel frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testMaxLocation(const double testDuration);

		/**
		 * Benchmarks the performance of function allowing to determine the location of a minimum value within a 1-channel frame.
		 * @param width The width of the frame to cast, with range [1, infinity)
		 * @param height The height of the frame to cast, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static void testMinLocation(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Benchmarks the performance of function allowing to determine the location of a maximum value within a 1-channel frame.
		 * @param width The width of the frame to cast, with range [1, infinity)
		 * @param height The height of the frame to cast, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static void testMaxLocation(const unsigned int width, const unsigned int height, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_MIN_MAX_H
