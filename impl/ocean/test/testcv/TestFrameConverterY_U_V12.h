/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_U_V_12_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_U_V_12_H

#include "ocean/test/testcv/TestCV.h"
#include "ocean/test/testcv/FrameConverterTestUtilities.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Y_U_V 12bit frame converter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameConverterY_U_V12
{
	public:

		/**
		 * Tests all Y_U_V12 frame conversion functions.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @param selector The test selector to control which tests to run
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector = TestSelector());

		/**
		 * Tests the Y_U_V12 to Y_U_V12 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12ToY_U_V12(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to BGR24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_FULL_RANGE to BGR24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to BGRA32 (full range) conversion with 6 bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to RGB24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to RGB24 (full range) conversion with 6 bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_FULL_RANGE to RGB24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to RGBA32 (full range) conversion with 6 bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 to YUV24 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12ToYUV24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 to YVU24 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12ToYVU24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to Y8_LIMITED_RANGE conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_FULL_RANGE to Y8_FULL_RANGE conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_LIMITED_RANGE to Y8_FULL_RANGE conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToY8FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12_FULL_RANGE to Y8_LIMITED_RANGE conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToY8LimitedRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 to Y_UV12 conversion.
		 * This conversion is not part of the conversion function map and accepts source planes with a pixel stride larger than 1, so the test is applied manually instead of via FrameConverterTestUtilities.
		 * @param width The width of the original frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the original frame in pixel, with range [2, infinity), must be even
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12ToY_UV12(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the Y_U_V12 to Y_UV12 conversion.
		 * @param ySource The y source plane, with (width + ySourcePaddingElements) * height elements, must be valid
		 * @param uSource The u source plane, with (width/2 + uSourcePaddingElements) * height/2 elements, must be valid
		 * @param vSource The v source plane, with (width/2 + vSourcePaddingElements) * height/2 elements, must be valid
		 * @param yTarget The y target plane, with (width + yTargetPaddingElements) * height elements, must be valid
		 * @param uvTarget The uv target plane, with (width + uvTargetPaddingElements) * height/2 elements, must be valid
		 * @param width The width of the frame in pixel, with range [2, infinity), must be even
		 * @param height The height of the frame in pixel, with range [2, infinity), must be even
		 * @param ySourcePaddingElements The number of padding elements at the end of each y-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param uSourcePaddingElements The number of padding elements at the end of each u-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param vSourcePaddingElements The number of padding elements at the end of each v-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param yTargetPaddingElements The number of padding elements at the end of each y-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param uvTargetPaddingElements The number of padding elements at the end of each uv-target row, in (uint8_t) elements, with range [0, infinity)
		 * @param ySourcePixelStride The stride between consecutive pixels in the y source plane, in elements, with range [1, infinity)
		 * @param uSourcePixelStride The stride between consecutive pixels in the u source plane, in elements, with range [1, infinity)
		 * @param vSourcePixelStride The stride between consecutive pixels in the v source plane, in elements, with range [1, infinity)
		 * @return True, if the target planes hold the expected values
		 */
		static bool validateY_U_V12ToY_UV12(const uint8_t* ySource, const uint8_t* uSource, const uint8_t* vSource, const uint8_t* yTarget, const uint8_t* uvTarget, const unsigned int width, const unsigned int height, const unsigned int ySourcePaddingElements, const unsigned int uSourcePaddingElements, const unsigned int vSourcePaddingElements, const unsigned int yTargetPaddingElements, const unsigned int uvTargetPaddingElements, const unsigned int ySourcePixelStride, const unsigned int uSourcePixelStride, const unsigned int vSourcePixelStride);

		/**
		 * Extracts one pixel from a Y_U_V12 source frame.
		 * @param frame The frame from which the pixel will be extracted, must be valid
		 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
		 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
		 * @param conversionFlag The conversion flag that will be applied, must be valid
		 * @return The vector holding the frame's color value at the specified location
		 */
		static MatrixD pixelFunctionY_U_V12ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_U_V_12_H
