/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Sobel filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterSobel
{
	friend class TestFrameFilterSobelMagnitude;

	public:

		/**
		 * Test all Sobel functions.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the horizontal and vertical 8 bit Sobel filter.
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
		 * Tests the horizontal and vertical 8 bit Sobel filter calculating three products based on the filter responses and handling a single row only.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHorizontalVertical3Squared1Channel8BitRow(const double testDuration);

		/**
		 * Tests the maximum absolute horizontal and vertical 8 bit Sobel filter.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool testHorizontalVerticalMaximumAbsolute8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 45 and 135 degree 8 bit Sobel filter.
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
		 * Tests the 0, 90, 45 and 135 degree 8 bit Sobel filter.
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
		 * Tests the comfort functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testComfort(const double testDuration, Worker& worker);

		/**
		 * Tests the pixel filter function determining the three squared Sobel filter responses.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFilterPixelCoreHorizontalVertical3Squared1Channel(const double testDuration);

	protected:

		/**
		 * Validates the horizontal and vertical 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'int8_t' or 'int16_t'
		 */
		template <typename TTarget>
		static bool validateHorizontalVerticalFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the 45 and 135 degree 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateDiagonalFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the horizontal, vertical, and diagonal 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 */
		static bool validateFilter8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Validates the maximum absolute horizontal and vertical 8 bit Sobel filter.
		 * @param frame The frame which to which the Sobel filter has been applied, must be valid
		 * @param response The frame with the Sobel responses, must be valid
		 * @return True, if succeeded
		 * @tparam TTarget The data type of the target responses, either 'uint8_t' or 'uint16_t'
		 */
		template <typename TTarget>
		static bool validateHorizontalVerticalMaximumAbsolute8BitPerChannel(const Frame& frame, const Frame& response);

		/**
		 * Returns the Sobel response for an image at a specified location.
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
int32_t TestFrameFilterSobel::filterResponse(const Frame& frame, const unsigned int x, unsigned int y, const unsigned int channelIndex)
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
			 * | -1  0  1 |
			 * | -2  0  2 |
			 * | -1  0  1 |
			 */
			return int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex]) * 2 - int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex]) * 2;
		}

		case 45u:
		{
			/**
			* 45 degree: Multiplication mask:
			* | -2   -1    0 |
			* | -1    0    1 |
			* |  0    1    2 |
			*/

			return int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]) * 2 - int32_t(frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex]) * 2;
		}

		case 90u:
		{
			/**
			 * 90 degree:
			 * | -1  -2  -1 |
			 * |  0   0   0 |
			 * |  1   2   1 |
			 */
			return int32_t(-frame.constpixel<uint8_t>(x - 1u, y - 1u)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex]) * 2 - int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex]) + int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]) * 2 + int32_t(frame.constpixel<uint8_t>(x + 1u, y + 1u)[channelIndex]);
		}

		case 135u:
		{
			/**
			 * 135 degree: Multiplication mask:
			 * |  0   -1    -2 |
			 * |  1    0    -1 |
			 * |  2    1     0 |
			 */
			return -int32_t(frame.constpixel<uint8_t>(x, y - 1u)[channelIndex]) - int32_t(frame.constpixel<uint8_t>(x + 1u, y - 1u)[channelIndex]) * 2 - int32_t(frame.constpixel<uint8_t>(x + 1u, y)[channelIndex])
					+ int32_t(frame.constpixel<uint8_t>(x - 1u, y)[channelIndex]) + int32_t(frame.constpixel<uint8_t>(x - 1u, y + 1u)[channelIndex]) * 2 + int32_t(frame.constpixel<uint8_t>(x, y + 1u)[channelIndex]);
		}

		default:
			break;
	}

	ocean_assert(false && "This should never happen!");
	return NumericT<int32_t>::minValue();
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SOBEL_H
