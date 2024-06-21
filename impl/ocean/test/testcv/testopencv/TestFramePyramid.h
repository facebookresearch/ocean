/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_PYRAMID_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_PYRAMID_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include "ocean/test/testcv/TestFrameShrinker.h"

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
 * This class implements performance tests for the frame pyramid of OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFramePyramid : protected Test::TestCV::TestFrameShrinker
{
	public:

		/**
		 * Benchmarks frame pyramid functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the performance of downsampling a frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testFrameDownsampling(const double testDuration);

		/**
		 * Benchmarks the performance of creating a frame pyramid.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testFramePyramid(const double testDuration);

		/**
		 * Benchmarks the performance of downsampling a frame.
		 * @param width The width of the frame to downsample, with range [2, infinity)
		 * @param height The height of the frame to downsample, with range [2, infinity)
		 * @param channels The nmber of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testFrameDownsampling(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Benchmarks the performance of creating a frame pyramid.
		 * @param width The width of the finest pyramid layer, with range [2, infinity)
		 * @param height The height of the finest pyramid layer, with range [2, infinity)
		 * @param channels The nmber of frame channels, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_PYRAMID_H
