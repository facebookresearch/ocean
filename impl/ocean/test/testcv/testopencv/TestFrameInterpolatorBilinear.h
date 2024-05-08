/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testcv/TestFrameInterpolatorBilinear.h"

#include <opencv2/core.hpp>

#include <type_traits>

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
		 * Benchmark the resizing function against OpenCV cv::resize() and AML variant of it
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

		/**
		 * OpenCV affine warp with modifications for the AML FaceTracker
		 *
		 * This function is a clone of fbsource/xplat/arfx/tracking/facetracker/Util/FastWarpAffine.h:warpAffine(...) (FBS: d543d4e)
		 *
		 * @param src input image.
		 * @param dstP output image that has the size dsize and the same type as src .
		 * @param M0 \f$3\times 3\f$ transformation matrix.
		 * @param dsize size of the output image.
		 * @param interpolation combination of interpolation methods (INTER_LINEAR or INTER_NEAREST) and the
		 */
		static void amlFacetrackerWarpAffine(const cv::Mat& src, cv::Mat& dstP, const cv::Mat& M0, cv::Size dsize, int interpolation);

		/**
		 * OpenCV affine warp with modifications for the AML FaceTracker
		 *
		 * This function is a clone of fbsource/xplat/arfx/tracking/facetracker/Util/ResizeImage.h:resize(...) (FBS: 16d716)
		 *
		 *
		 * @param _src The source frame
		 * @param _dst The destination frame; it has the size dsize (when it is non-zero) or the size computed from _src.size(), fx, and fy; the type of dst is the same as of _src.
		 * @param dsize
		 * @param inv_scale_x
		 * @param inv_scale_y
		 * @param interpolation
		 */
		static void amlFacetrackerResize(const cv::Mat& _src, cv::Mat& _dst, cv::Size dsize, double inv_scale_x = 0, double inv_scale_y = 0, int interpolation = 1);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FRAME_INTERPOLATOR_BILINEAR_H
