/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GRADIENT_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GRADIENT_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a gradient filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterGradient
{
	public:

		/**
		 * Tests all mean filter functions.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the horizontal and vertical gradient filter for arbitrary data types.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testHorizontalVertical(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the horizontal and vertical gradient filter for a sub-frame with arbitrary data types.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testHorizontalVerticalSubFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the squared magnitude gradient filter for arbitrary data types.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testHorizontalVerticalMagnitudeSquared(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the horizontal and vertical gradient filter for arbitrary data types.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param channels The number of channels the frame has, with range [1, 4]
		 * @param multiplicationFactor The factor to be multiplied to the target, with range (-infinity, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target filter response
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TSource, typename TTarget, bool tNormalizeByTwo>
		static bool testHorizontalVertical(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker);

		/**
		 * Tests the squared magnitude gradient filter for arbitrary data types.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param channels The number of channels the frame has, with range [1, 4]
		 * @param multiplicationFactor The factor to be multiplied to the target, with range (-infinity, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target filter response
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TSource, typename TTarget, bool tNormalizeByTwo>
		static bool testHorizontalVerticalMagnitudeSquared(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker);

		/**
		 * Tests the horizontal and vertical gradient filter for arbitrary data types for a subframe.
		 * @param width The width of source and target buffer in pixel, with range [3, infinity)
		 * @param height The height of source and target buffer in pixel, with range [3, infinity)
		 * @param channels The number of channels the frame has, with range [1, 4]
		 * @param multiplicationFactor The factor to be multiplied to the target, with range (-infinity, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target filter response
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TSource, typename TTarget, bool tNormalizeByTwo>
		static bool testHorizontalVerticalSubFrame(const unsigned int width, const unsigned int height, const unsigned int channels, const TTarget multiplicationFactor, const double testDuration, Worker& worker);

		/**
		 * Tests the lined integral image for the horizontal 1x2 gradient filter.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam T The data type of the elements in the source image
		 * @tparam TIntegral The data type of the elements in the integral image
		 */
		template <typename T, typename TIntegral>
		static bool testFilterHorizontal1x2LinedIntegralImage(const double testDuration);

		/**
		 * Tests the lined integral image for the vertical 2x1 gradient filter.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam T The data type of the elements in the source image
		 * @tparam TIntegral The data type of the elements in the integral image
		 */
		template <typename T, typename TIntegral>
		static bool testFilterVertical2x1LinedIntegralImage(const double testDuration);

	private:

		/**
		 * Tests the validation of the horizontal and vertical gradient filter.
		 * @param source The source frame which will be filtered, must be valid
		 * @param target The target frame, must be valid
		 * @param multiplicationFactor The factor to be multiplied to the target
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target filter response
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TSource, typename TTarget, bool tNormalizeByTwo>
		static bool validationHorizontalVertical(const Frame& source, const Frame& target, const TTarget multiplicationFactor = TTarget(1));

		/**
		 * Tests the validation of the squared magnitude gradient filter.
		 * @param source The source frame which will be filtered, must be valid
		 * @param target The target frame, must be valid
		 * @param multiplicationFactor The factor to be multiplied to the target
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the target filter response
		 * @tparam tNormalizeByTwo True, to normalize the subtraction result by two; False, to simply determine the subtraction result
		 */
		template <typename TSource, typename TTarget, bool tNormalizeByTwo>
		static bool validationHorizontalVerticalMagnitudeSquared(const Frame& source, const Frame& target, const TTarget multiplicationFactor = TTarget(1));
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GRADIENT_H
