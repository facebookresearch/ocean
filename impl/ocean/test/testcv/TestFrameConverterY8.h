/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_8_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_8_H

#include "ocean/test/testcv/TestCV.h"
#include "ocean/test/testcv/FrameConverterTestUtilities.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Y8 frame converter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameConverterY8
{
	public:

		/**
		 * Tests all Y8 bit frame conversion functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 full range to BGR24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8FullRangeToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 full range to RGB24 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8FullRangeToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 full range to RGBA32 (full range) conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8FullRangeToRGBA32(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 to Y8 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8ToY8(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 to Y8 conversion with gamma compression/correction.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8ToY8GammaLUT(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to Y8 full range conversion with 6-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToY8FullRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to Y8 full range conversion with 10-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToY8FullRangePrecision10Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 full range to Y8 limited range conversion with 6-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8FullRangeToY8LimitedRangePrecision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 full range to Y8 limited range conversion with 10-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8FullRangeToY8LimitedRangePrecision10Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to RGB24 (full range) conversion with 10-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToRGB24Precision10Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to RGBA32 (full range) conversion with 10-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToRGBA32Precision10Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to RGB24 (full range) conversion with 6-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToRGB24Precision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the Y8 limited range to RGBA32 (full range) conversion with 6-bit precision.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testY8LimitedRangeToRGBA32Precision6Bit(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

	protected:

		/**
		 * Extracts one pixel from a Y8 source frame and applies gamma correction.
		 * @param frame The frame from which the pixel will be extracted, must be valid
		 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
		 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
		 * @param conversionFlag The conversion flag that will be applied, must be valid
		 * @return The vector holding the frame's color value at the specified location
		 */
		static MatrixD pixelFunctionY8Gamma(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_Y_8_H
