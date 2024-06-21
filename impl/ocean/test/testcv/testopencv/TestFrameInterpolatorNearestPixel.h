/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H

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
 * This class benchmarks the performance of nearest-neighbor interpolation.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFrameInterpolatorNearestPixel
{
	public:

		/**
		 * Benchmarks nearest-neighbor interpolation functions.
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
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_NEAREST_PIXEL_H
