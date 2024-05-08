/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_LAPLACE_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_LAPLACE_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the Laplace frame filter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterLaplace
{
	public:

		/**
		 * Invokes all tests.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the Laplace filter for 1 channel.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of each frame element
		 * @tparam TResponse The data type of each response element
		 */
		template <typename T, typename TResponse>
		static bool test1Channel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the variance function.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVariance1Channel(const unsigned int width, const unsigned int height, const double testDuration);

	protected:

		/**
		 * Validates the Laplace filter for 1-plane, 1-channel images.
		 * @param frame The frame to which the filter is applied, must be valid
		 * @param response The resulting filter response, must be valid
		 * @param borderPixelZero True, if border pixels must be zero; False, if border pixels need to apply a subset of the filter
		 * @return True, if succeeded
		 * @tparam T The data type of each frame element
		 * @tparam TResponse The data type of each response element
		 */
		template <typename T, typename TResponse>
		static bool validate(const Frame& frame, const Frame& response, const bool borderPixelZero);

		/**
		 * Determines the response for a border pixel applying a subset of the filter.
		 * @param frame The frame to which the filter is applied, must be valid
		 * @param x The horizontal center position of the filter, in pixel, with range [0, frame.width() - 1]
		 * @param y The vertical center position of the filter, in pixel, with range [0, frame.height() - 1]
		 * @return The response
		 * @tparam T The data type of each frame element
		 * @tparam TFloat The float data type for the response
		 */
		template <typename T, typename TFloat>
		static TFloat responseBorderPixel(const Frame& frame, const unsigned int x, const unsigned int y);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_LAPLACE_H
