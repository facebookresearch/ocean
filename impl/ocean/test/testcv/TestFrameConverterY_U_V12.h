/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_U_V_12_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_U_V_12_H

#include "ocean/test/testcv/TestCV.h"
#include "ocean/test/testcv/TestFrameConverter.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"

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
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

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
		 * Tests the Y_U_V12 (limited range) to BGR24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToBGR24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (full range) to BGR24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToBGR24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (limited range) to BGRA32 (full range) conversion with 6 bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (limited range) to RGB24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToRGB24FullRange(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (limited range) to RGB24 (full range) conversion with 6 bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (full range) to RGB24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12FullRangeToRGB24FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y_U_V12 (limited range) to RGBA32 (full range) conversion with 6 bit precision.
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
		 * Tests the Y_U_V12 to Y8 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testY_U_V12ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag conversionFlag, const double testDuration, Worker& worker);

	protected:

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
