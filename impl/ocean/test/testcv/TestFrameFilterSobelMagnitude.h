/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_MAGNITUDE_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_MAGNITUDE_H

#include "ocean/test/testcv/TestCV.h"
#include "ocean/test/testcv/TestFrameFilterSobel.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a scharr magnitude filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterSobelMagnitude
{
	public:

		/**
		 * Invokes all tests for the Sobel Magnitude filter.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 0, and 90 degree 8 bit Sobel magnitude filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/8, 'int16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testHorizontalVerticalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 45, and 135 degree 8 bit Sobel magnitude filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/8, 'int16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testDiagonalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 0, 90, 45, and 135 degree 8 bit Sobel magnitude filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/8, 'int16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 0, and 90 degree 8 bit Sobel magnitude filter storing only one response value per pixel.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/8, 'int16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testHorizontalVerticalFilterTo1Response8Bit(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the horizontal and vertical 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilterHorizontalVerticalAs1Channel8Bit(const Frame& frame, const Frame& response);

		/**
		 * Validates the diagonal (45 degree and 135 degree) 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilterDiagonalAs1Channel8Bit(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal, vertical, and diagonal 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilterAs1Channel8Bit(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal and vertical 8 bit Sobel filter storing only one response value per pixel.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilterTo1Response8Bit(const Frame& frame, const Frame& response);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_MAGNITUDE_H
