/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SEPARABLE_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SEPARABLE_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements tests for the AdvancedFrameFilterSeparable class.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedFrameFilterSeparable
{
	public:

		/**
		 * Invokes all tests.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the filter function.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testFilter(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the filter function.
		 * @param width The width of the test frame in pixel, with range [horizontalFilterSize/2, infinity)
		 * @param height The height of the test frame in pixel, with range [horizontalFilterSize/2, infinity)
		 * @param horizontalFilterSize The size of the horizontal filter, in pixel, with range [3, infinity), must be odd
		 * @param verticalFilterSize The size of the horizontal filter, in pixel, with range [3, infinity), must be odd
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 * @tparam TFilter The data type of the filter factors
		 */
		template <typename T, typename TFilter>
		static bool testFilter(const unsigned int width, const unsigned int height, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker);

		/**
		 * Tests the filter function.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testFilterInPlace(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the filter function.
		 * @param width The width of the test frame in pixel, with range [horizontalFilterSize/2, infinity)
		 * @param height The height of the test frame in pixel, with range [horizontalFilterSize/2, infinity)
		 * @param horizontalFilterSize The size of the horizontal filter, in pixel, with range [3, infinity), must be odd
		 * @param verticalFilterSize The size of the horizontal filter, in pixel, with range [3, infinity), must be odd
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 * @tparam TFilter The data type of the filter factors
		 */
		template <typename T, typename TFilter>
		static bool testFilterInPlace(const unsigned int width, const unsigned int height,const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the filter response.
		 * @param source The source frame which has been used, must be valid
		 * @param sourceMask The source mask which has been used, must be valid
		 * @param target The filter result to verify, must be valid
		 * @param targetMask The target mask to verify, must be valid
		 * @param horizontalFilters The horizontal (separable) filter factors which have been used, must be odd factors
		 * @param verticalFilters The vertical (separable) filter factors which have been used, must be odd factors
		 * @param maskValue The value of an invalid mask pixel which will be skipped during filtering, with range [0, 255]
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool validateFilter(const Frame& source, const Frame& sourceMask, const Frame& target, const Frame& targetMask, const std::vector<float>& horizontalFilters, const std::vector<float>& verticalFilters, const uint8_t maskValue);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SEPARABLE_H
