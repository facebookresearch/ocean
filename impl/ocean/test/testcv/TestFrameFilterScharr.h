/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SCHARR_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SCHARR_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Scharr filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterScharr
{
	friend class TestFrameFilterScharrMagnitude;

	public:

		/**
		 * Test all Scharr functions.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 0, and 90 degree 8 bit Scharr filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/32, 'int16_t' responses are not normalized.
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
		 * Tests the 45, and 135 degree 8 bit Scharr filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/32, 'int16_t' responses are not normalized.
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
		 * Tests the 0, 90, 45 and 135 degree 8 bit Scharr filter.
		 * This function supports 'int8_t' and 'int16_t' responses, 'int8_t' responses are normalized by 1/32, 'int16_t' responses are not normalized.
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
		 * Tests the 0, and 90 degree 8 bit Scharr filter determining the maximum of the absolute responses.
		 * This function supports 'uint8_t' and 'uint16_t' responses, 'uint8_t' responses are normalized by 1/16, 'uint16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 0, 90, 45, and 135 degree 8 bit Scharr filter determining the maximum of the absolute responses.
		 * This function supports 'uint8_t' and 'uint16_t' responses, 'uint8_t' responses are normalized by 1/16, 'uint16_t' responses are not normalized.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testMaximumAbsoluteFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the horizontal and vertical 8 bit Scharr filter.
		 * @param frame The frame which to which the Scharr filter has been applied, must be valid
		 * @param response The frame with the Scharr responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateHorizontalVerticalFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the diagonal (45 degree and 135 degree) 8 bit Scharr filter.
		 * @param frame The frame which to which the Scharr filter has been applied, must be valid
		 * @param response The frame with the Scharr responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateDiagonalFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal, vertical, and diagonal 8 bit Scharr filter.
		 * @param frame The frame which to which the Scharr filter has been applied, must be valid
		 * @param response The frame with the Scharr responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal and vertical maximum absolute 8 bit Scharr filter.
		 * @param frame The frame which to which the Scharr filter has been applied, must be valid
		 * @param response The frame with the Scharr responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateHorizontalVerticalMaximumAbsoluteFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal, vertical, and diagonal maximum absolute 8 bit Scharr filter.
		 * @param frame The frame which to which the Scharr filter has been applied, must be valid
		 * @param response The frame with the Scharr responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateMaximumAbsoluteFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Returns the Scharr response for an image at a specified location.
		 * @param frame The frame in which the response will be determined, must be valid
		 * @param x The horizontal center location of the filter, with range [0, frame.width() - 1]
		 * @param y The vertical center location of the filter, with range [0, frame.height() - 1]
		 * @param channelIndex The index of the channel for which the response will be determined, with range [0, frame.channels() - 1]
		 * @return The resulting response
		 * @tparam tAngle The angle of the filter, one of the following values {0, 45, 90, 135}
		 */
		template <unsigned int tAngle>
		static int32_t filterResponse(const Frame& frame, const unsigned int x, unsigned int y, const unsigned int channelIndex);
};

template <unsigned int tAngle>
int32_t TestFrameFilterScharr::filterResponse(const Frame& frame, const unsigned int x, unsigned int y, const unsigned int channelIndex)
{
	static_assert(tAngle == 0u || tAngle == 45u || tAngle == 90u || tAngle == 135u, "Invalid angle!");

	ocean_assert(frame.isValid() && x < frame.width() && y < frame.height() && channelIndex < frame.channels());

	if (x == 0u || y == 0u || x + 1u == frame.width() || y + 1u == frame.height())
	{
		return 0;
	}

	switch (tAngle)
	{
		case 0u:
		{
			/**
			 * 0 degree:
			 * |  -3   0   3 |
			 * | -10   0  10 |
			 * |  -3   0   3 |
			 */
			return int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex]) * 3 - int32_t(frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex]) * 3
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]) * 3 - int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex]) * 3
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex]) * 10 - int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex]) * 10;
		}

		case 45u:
		{
			/**
			* 45 degree: Multiplication mask:
			* | -10   -3    0 |
			* |  -3    0    3 |
			* |   0    3   10 |
			*/

			return int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex]) * 3 - int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex]) * 3
					+ int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]) * 3 - int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex]) * 3
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]) * 10 - int32_t(frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex]) * 10;
		}

		case 90u:
		{
			/**
			 * 90 degree:
			 * | -3  -10  -3 |
			 * |  0   0    0 |
			 * |  3   10   3 |
			 */
			return int32_t(frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex]) * -3 + int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex]) * -10 + int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex]) * -3
					+ int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex]) * 3 + int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]) * 10 + int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]) * 3;
		}

		case 135u:
		{
			/**
			 * 135 degree: Multiplication mask:
			 * |  0   -3   -10 |
			 * |  3    0    -3 |
			 * | 10    3     0 |
			 */
			return int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex]) * -3 + int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex]) * -10 + int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex]) * -3
					+ int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex]) * 3 + int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex]) * 10 + int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]) * 3;
		}

	}

	ocean_assert(false && "This should never happen!");
	return NumericT<int32_t>::minValue();
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SCHARR_H
