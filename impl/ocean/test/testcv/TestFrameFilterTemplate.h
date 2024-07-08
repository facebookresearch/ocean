/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_TEMPLATE_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_TEMPLATE_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/cv/FrameFilterTemplate.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame filter template test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterTemplate
{
	public:

		/**
		 * Tests the entire filter template functions.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the filter response of the template filter for frames with 8 bit per channel and 8 bit integer response.
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testFilter8BitPerChannelTo8BitInteger(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the filter response of the template filter for frames with 8 bit per channel and 32 bit float response.
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testFilter8BitPerChannelTo32BitFloat(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the filter response of the template filter using a normalization factor for frames with 8 bit per channel and 32 bit float response.
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testFilterWithFactor8BitPerChannelTo32BitFloat(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the pixel response function of the template filter for frames with 8 bit per channel.
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPixel8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

	protected:

		/**
		 * Validates the template filter response for frames with 8 bit per channel.
		 * @param frame The frame that is filtered
		 * @param filtered Filter responses that will be verified
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param filterPattern Nine filter factors that are applied as filter
		 * @param normalization Denominator normalization factor for each filter response
		 * @param bias Optional bias value allowing to improve integer rounding accuracy
		 * @param direction Direction of the filter
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam TResponse Data type of the filter responses
		 * @tparam TNormalization Data type of the normalization factor (the denominator)
		 */
		template <typename TResponse, typename TNormalization>
		static bool validateFilter8BitPerChannel(const uint8_t* frame, const TResponse* filtered, const unsigned int width, const unsigned int height, const unsigned int channels, const int filterPattern[9], const TNormalization normalization, const TNormalization bias, const CV::PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the template filter response for frames with 8 bit per channel.
		 * @param frame The frame that is filtered
		 * @param filtered Filter responses that will be verified
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels Number of zipped data channels of the frame, with range [1, 4]
		 * @param filterPattern Nine filter factors that are applied as filter
		 * @param factor Denominator normalization factor for each filter response
		 * @param direction Direction of the filter
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam TResponse Data type of the filter responses
		 * @tparam TNormalizationFactor Data type of the normalization factor (the nominator)
		 */
		template <typename TResponse, typename TNormalizationFactor>
		static bool validateFilterWithFactor8BitPerChannel(const uint8_t* frame, const TResponse* filtered, const unsigned int width, const unsigned int height, const unsigned int channels, const int filterPattern[9], const TNormalizationFactor factor, const CV::PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the template pixel response for frames with 8 bit per channel.
		 * @param frame The test frame to filter
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x The horizontal filter position in pixel, with range [0, width)
		 * @param y The vertical filter position in pixel, with range [0, height)
		 * @param channels The number of channels the provided frame has, with range [1, 4]
		 * @param direction The direction of the filter
		 * @tparam TFilterTemplate The filter template to be used for filtering
		 * @tparam TResponse The data type of the filter response
		 * @tparam TNormalization The data type of the normalization factor (of the denominator)
		 * @tparam tNormalization The normalization factor (the denominator)
		 * @tparam tBias The optional bias value which can be used to improve an integer rounding result
		 * @return True, if succeeded
		 */
		template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias>
		static bool validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int channels, const CV::PixelDirection direction);

		/**
		 * Validates the template pixel response for frames with 8 bit per channel.
		 * @param frame The test frame to filter
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x The horizontal filter position in pixel, with range [0, width)
		 * @param y The vertical filter position in pixel, with range [0, height)
		 * @param direction The direction of the filter
		 * @tparam TFilterTemplate The filter template to be used for filtering
		 * @tparam TResponse The data type of the filter response
		 * @tparam TNormalization The data type of the normalization factor (of the denominator)
		 * @tparam tNormalization The normalization factor (the denominator)
		 * @tparam tBias The optional bias value which can be used to improve an integer rounding result
		 * @tparam tChannels The number of channels the provided frame has, with range [1, 4]
		 * @return True, if succeeded
		 */
		template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels>
		static bool validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const CV::PixelDirection direction);

		/**
		 * Validates the template pixel response for frames with 8 bit per channel.
		 * @param frame The test frame to filter
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x The horizontal filter position in pixel, with range [0, width)
		 * @param y The vertical filter position in pixel, with range [0, height)
		 * @tparam TFilterTemplate The filter template to be used for filtering
		 * @tparam TResponse The data type of the filter response
		 * @tparam TNormalization The data type of the normalization factor (of the denominator)
		 * @tparam tNormalization The normalization factor (the denominator)
		 * @tparam tBias The optional bias value which can be used to improve an integer rounding result
		 * @tparam tChannels The number of channels the provided frame has, with range [1, 4]
		 * @tparam tDirection The direction of the filter
		 * @return True, if succeeded
		 */
		template <typename TFilterTemplate, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, CV::PixelDirection tDirection>
		static bool validateFilterPixel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y);

		/**
		 * Determines the template filter response for frames with 8 bit per channel.
		 * @param frame The test frame to filter
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x The horizontal filter position in pixel, with range [0, width)
		 * @param y The vertical filter position in pixel, with range [0, height)
		 * @param channels The number of channels the provided frame has
		 * @param factors The 9 filter factors of the filter
		 * @param normalization The normalization value applied as denominator
		 * @param bias Optional bias value which can be used to improve the integer round accuracy
		 * @param direction The direction of the filter
		 * @param response The resulting filter responses, one for each channel
		 * @tparam TResponse The data type of the filter response
		 * @tparam TNormalization The data type of the normalization value
		 */
		template <typename TResponse, typename TNormalization>
		static void determinePixelResponse(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int channels, const int factors[9], const TNormalization normalization, const TNormalization bias, const CV::PixelDirection direction, TResponse* response);

		/**
		 * Returns all individual directions.
		 * @return A vector holding all directions
		 */
		static const CV::PixelDirections& directions();

		/**
		 * Returns all individual directions as readable string.
		 * @return A vector holding all strings
		 */
		static const std::vector<std::string>& directionStrings();
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_TEMPLATE_H
