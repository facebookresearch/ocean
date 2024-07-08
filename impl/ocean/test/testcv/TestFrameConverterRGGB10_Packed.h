/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_RGGB_10_PACKED_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_RGGB_10_PACKED_H

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
 * This class implements a RGGB10_PACKED frame converter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameConverterRGGB10_Packed
{
	private:

		/**
		 * Definition of a functor class to access single pixels of an frame with pixel format RGGB10_PACKED
		 */
		class PixelFunctorRGGB10_Packed
		{
			public:

				/**
				 * Constructor to specify black level, white balance, and gamma
				 * @param blackLevel The black level value that is subtracted from each element of the raw image before any other operation, range: [0, 1024)
				 * @param whiteBalance Pointer to the white balancing scalars of the red, green, and blue channel (in that order), range: [0, infinity), must be valid and have at least 3 elements
				 * @param gamma The gamma value that each pixel will be encoded with, range: [0, infinity), will be ignored if value is < 0
				 */
				inline PixelFunctorRGGB10_Packed(const uint16_t blackLevel, const double* whiteBalance, const double gamma);

				/**
				 * Extracts one pixel from a RGGB10_PACKED source frame and subtracts the black level and applies white balancing as well as gamma
				 * The signature of this function intentionally matches that of `TestFrameConverter::FunctionPixelValue`.
				 * @param frame The frame from which the pixel will be extracted, must be valid
				 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
				 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
				 * @param conversionFlag The conversion flag that will be applied, must be valid
				 * @return The vector holding the frame's color value at the specified location
				 * @sa TestFrameConverter::FunctionPixelValue
				 */
				inline MatrixD operator()(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag) const;

				/**
				 * Returns bit element value of a pixel of RGGB10_PACKED frame as a double clamped to [0, 1023]
				 * The lookup can be done for one top row and bottom row outside of the frame, this row is then mirrored back into the frame.
				 * @param frame The frame from which the value will be extracted, must be valid
				 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
				 * @param y The vertical pixel location within the frame with one additional top and bottom row, with range [-1, frame.height()]
				 * @param blackLevel Optional black level that will be subtracted from the pixel value (with clamping at 0), range: [0, 1024)
				 * @param whiteBalance Optional white balance scalar that will be multiplied with the value of the element, range: [0 infinity)
				 * @return The value of the element as a double, clamped to [0, 1023]
				 */
				static double elementValue(const Frame& frame, const unsigned int x, const int y, const uint16_t blackLevel = 0u, const double whiteBalance = 1.0);

				/**
				 * Extracts one pixel from a RGGB10_PACKED source frame.
				 * @param frame The frame from which the pixel will be extracted, must be valid
				 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
				 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
				 * @param conversionFlag The conversion flag that will be applied, must be valid
				 * @return The vector holding the frame's color value at the specified location
				 */
				static inline MatrixD pixelFunctionRGGB10_Packed(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag);

				/**
				 * Extracts one pixel from a RGGB10_PACKED source frame and subtracts the black level and applies white balancing as well as gamma
				 * @param frame The frame from which the pixel will be extracted, must be valid
				 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
				 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
				 * @param conversionFlag The conversion flag that will be applied, must be valid
				 * @param blackLevel The black level value that is subtracted from each element of the raw image before any other operation, range: [0, 1024)
				 * @param whiteBalance Pointer to the white balancing scalars of the red, green, and blue channel (in that order), range: [0, infinity), must be valid and have at least 3 elements
				 * @param gamma The gamma value that each pixel will be encoded with, range: [0, infinity)
				 * @return The vector holding the frame's color value at the specified location
				 */
				static MatrixD pixelFunctionRGGB10_PackedWithBlackLevelWhiteBalanceGamma(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag, const uint16_t blackLevel, const double* whiteBalance, const double gamma);

			protected:

				/// The black level value that is subtracted from each element of the raw image before any other operation, range: [0, 1024)
				uint16_t blackLevel_ = 0u;

				/// The white balancing scalars of the red, green, and blue channel (in that order), range: [0, infinity), will be ignored channel-wise for values < 0
				double whiteBalance_[3] = {0.0, 0.0, 0.0};

				/// The gamma value that each pixel will be encoded with, range: [0, infinity)
				double gamma_ = 0.0;
		};

	public:

		/**
		 * Tests all RGGB10_PACKED bit frame conversion functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the RGGB10_PACKED to BGR24 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testRGGB10_PackedToBGR24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the RGGB10_PACKED to RGB24 conversion.
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testRGGB10_PackedToRGB24(const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);

		/**
		 * Tests the RGGB10_PACKED to RGB24 conversion with black-level subtraction, white balancing, and gamma encoding.
		 * @param randomGenerator The random generator to be used
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag The conversion flag that has been applied during conversion
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testConvertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(RandomGenerator& randomGenerator, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const double testDuration, Worker& worker);
};

inline TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::PixelFunctorRGGB10_Packed(const uint16_t blackLevel, const double* whiteBalance, const double gamma) :
	blackLevel_(blackLevel),
	whiteBalance_{whiteBalance[0], whiteBalance[1], whiteBalance[2]},
	gamma_(gamma)
{
	ocean_assert(whiteBalance_[0] >= 0.0 && whiteBalance[1] >= 0.0 && whiteBalance_[2] >= 0.0);
	ocean_assert(gamma_ >= 0.0);
}

inline MatrixD TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::operator()(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag) const
{
	return pixelFunctionRGGB10_PackedWithBlackLevelWhiteBalanceGamma(frame, x, y, conversionFlag, blackLevel_, whiteBalance_, gamma_);
}


inline MatrixD TestFrameConverterRGGB10_Packed::PixelFunctorRGGB10_Packed::pixelFunctionRGGB10_Packed(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	constexpr uint16_t blackLevel = 0u;
	constexpr double whiteBalance[3] = {1.0, 1.0, 1.0};
	constexpr double gamma = 1.0;

	return pixelFunctionRGGB10_PackedWithBlackLevelWhiteBalanceGamma(frame, x, y, conversionFlag, blackLevel, whiteBalance, gamma);
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_RGGB_10_PACKED_H
