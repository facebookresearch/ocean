/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_CANNY_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_CANNY_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a canny filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterCanny
{
	public:

		/**
		 * Test functions for the Canny edge detector
		 * @param width The width of the test frame in pixels, with range [3, infinity)
		 * @param height The height of the test frame in pixels, with range [3, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the Canny edge detector with a Scharr filter
		 * @param performanceWidth The width of the test frame in pixels used for performance measurements, with range [3, infinity)
		 * @param performanceHeight The height of the test frame in pixels used for performance measurements, with range [3, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TFilter The underlying type for responses of the Scharr filter, may be either `int8_t` or `int16_t`.
		 */
		template <typename TFilter>
		static bool testFilterCannyScharr(const unsigned int performanceWidth, const unsigned int performanceHeight, const double testDuration, Worker& worker);

		/**
		 * Validates the result of the Canny edge detector with Scharr filter.
		 * @param original The original gray scale image, must be valid
		 * @param filtered The filtered gray scale image, must be valid
		 * @param lowThreshold The threshold below which edge candidates are immediately rejected, range: [0, highTreshold)
		 * @param highThreshold The threshold above which edge candidates, which are also local maxima, are immediately accepted as a strong edge, range: (lowThreshold, maxThreshold] where maxThreshold is 127 for TFilter=int8_t and 4080 for TFilter=int16_t
		 * @return True, if succeeded
		 * @tparam TFilter The underlying type for responses of the Scharr filter, may be either `int8_t` or `int16_t`.
		 */
		template <typename TFilter>
		static bool validationCannyFilterScharr(const Frame& original, const Frame& filtered, const TFilter lowThreshold, const TFilter highThreshold);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_CANNY_H
