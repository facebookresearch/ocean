/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GAUSSIAN_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GAUSSIAN_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the Gaussian image blur filter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterGaussian
{
	public:

		/**
		 * Tests all Gaussian filter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the conversion function between filter size and sigma.
		 * @return True, if succeeded
		 */
		static bool testFilterSizeSigmaConversion();

		/**
		 * Tests the calculation of filter factors.
		 * @return True, if succeeded
		 */
		static bool testFilterFactors();

		/**
		 * Tests extreme (small) frame dimensions.
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testExtremeDimensions(Worker& worker);

		/**
		 * Tests extreme (small) frame dimensions.
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param filterSize The size of the filter kernel, with range [1, infinity), must be odd
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testExtremeDimensions(const unsigned int channels, const unsigned int filterSize, Worker& worker);

		/**
		 * Tests the performance of the frame filter for normal frame dimensions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testNormalDimensions(const double testDuration, Worker& worker);

		/**
		 * Tests the filter function for a given image resolution.
		 * @param width The width of the test frame in pixel, with range [tFilterSize, infinity)
		 * @param height The height of the test frame in pixel, with range [tFilterSize, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param filterSize The size of the filter kernel, with range [1, infinity), must be odd
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testFilter(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker);

		/**
		 * Tests the performance of the frame filter when using a reusable memory object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testReusableMemory(const double testDuration);

		/**
		 * Tests the performance of the frame filter when using a reusable memory object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 */
		template <typename T>
		static bool testReusableMemoryComfort(const double testDuration);

		/**
		 * Tests the in-place Gaussian blur filter.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testInplace(const double testDuration, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_GAUSSIAN_H
