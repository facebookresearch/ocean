/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H

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
 * This class benchmarks the performance of bilinear interpolation.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFrameInterpolatorBilinear
{
	public:

		/**
		 * Benchmarks bilinear interpolation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void test(const double testDuration);

	private:

		/**
		 * Benchmarks the homography-based interpolation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testHomography(const double testDuration);

		/**
		 * Benchmarks the interpolation function based on affine transformations.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testAffine(const double testDuration);

		/**
		 * Benchmark test for image resizing
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @tparam T Data type to be tested (can be either unsigned char, float)
		 */
		 template <typename T>
		static void testResize(const double testDuration);

		/**
		 * Benchmarks the homography-based interpolation for a specific frame type.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Benchmarks the interpolation function based on affine transformations for a specific frame type.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 */
		static void testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Benchmark the resizing functions of Ocean against OpenCV cv::resize()
		 * @param sourceWidth The width of the source frame in pixels, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixels, with range [1, infinity)
		 * @param targetWidth The width of the target frame in pixels, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixels, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam T Data type to be tested (can be either unsigned char, float)
		 */
		template <typename T>
		static void testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double testDuration);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
