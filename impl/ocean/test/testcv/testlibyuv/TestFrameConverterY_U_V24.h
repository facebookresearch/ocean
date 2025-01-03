/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_Y_U_V_24_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_Y_U_V_24_H

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

/**
 * This class tests/benchmarks the performance of the frame converter function of libyuv for Y_U_V24 frames.
 * @ingroup testcvlibyuv
 */
class OCEAN_TEST_CV_LIBYUV_EXPORT TestFrameConverterY_U_V24
{
	public:

		/**
		 * Benchmarks the performance of libyuv agains Ocean for frame converter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param skipValidation True, to skip the validation
		 */
		static void test(const double testDuration, const bool skipValidation = false);

		/**
		 * Benchmarks the conversion from Y_U_V24 frames to BGRA32 frames.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param skipValidation True, to skip the validation
		 * @return True, if succeeded
		 */
		static bool testConvertY_U_V24ToBGRA32Precision6Bit(const double testDuration, const bool skipValidation = false);

	protected:

		/**
		 * Randomizes the content of a given Y_U_V24 frame while setting 2x2 areas of the y-plane and v-plane to the same value.
		 * @param frame The frame to be randomize, with even pixel resolution
		 */
		static void specialRandomizeFrame(Frame& frame);

		/**
		 * Extracts one pixel from a Y_U_V24 source frame.
		 * @param frame The frame from which the pixel will be extracted, must be valid
		 * @param x The horizontal pixel location within the frame, with range [0, frame.width() - 1]
		 * @param y The vertical pixel location within the frame, with range [0, frame.height() - 1]
		 * @param conversionFlag The conversion flag that will be applied, must be valid
		 * @return The vector holding the frame's color value at the specified location
		 */
		static MatrixD pixelFunctionY_U_V24ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_Y_U_V_24_H
