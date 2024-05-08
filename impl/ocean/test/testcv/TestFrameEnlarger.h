/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_ENLARGER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_ENLARGER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements frame enlarger test functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameEnlarger
{
	public:

		/**
		 * Tests the entire frame enlarger functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the add border function using a defined border color.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool testAddBorder(const double testDuration);

		/**
		 * Tests the add border function using the nearest pixel as color value.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool testAddBorderNearestPixel(const double testDuration);

		/**
		 * Tests the frame enlarger mirroring the frame's content.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool testAddBorderMirrored(const double testDuration);

		/**
		 * Tests upscaling of frames by a factor of two.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameMultiplyByTwo(const double testDuration, Worker& worker);

		/**
		 * Tests upscaling of frames by a factor of two for specific image sizes and number of channels
		 * @param width The width of the test frame in pixels used for performance measurements, with range [1, infinity)
		 * @param height The height of the test frame in pixels used for performance measurements, with range [1, infinity)
		 * @param channels The number of channels of the test frame, range: [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameMultiplyByTwo(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the add border function using the nearest pixel as color and alpha set to full transparency
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddTransparentBorder(const double testDuration);

	private:

		/**
		 * Validates the function adding a border with static color.
		 * @param original The original frame, must be valid
		 * @param enlarged The enlarged frame to validate, must be valid
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @param color The color to be used, one value for each channel
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool validateAddBorder(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const T* color);

		/**
		 * Validates the add border function using the nearest pixel as color value.
		 * @param original The original frame, must be valid
		 * @param enlarged The enlarged frame to validate, must be valid
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool validateAddBorderNearestPixel(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

		/**
		 * Validates the add border function mirroring the frame's content.
		 * @param original The original frame, must be valid
		 * @param enlarged The enlarged frame to validate, must be valid
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool validateAddBorderMirrored(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

		/**
		 * Validates the upscaling of frames by factor two.
		 * @param original The original source frame, must be valid
		 * @param enlarged The enlarged target frame that will be validated, must be valid
		 * @return True, if succeeded
		 */
		static bool validationMultiplyByTwo(const Frame& original, const Frame& enlarged);

		/**
		 * Validates the function adding a border with static color.
		 * @param original The original frame, must be valid
		 * @param enlarged The enlarged frame to validate, must be valid
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @param transparentIs0xFF If true transparency will be defined as 0xFF, otherwise 0x00
		 * @return True, if succeeded
		 */
		static bool validateAddTransparentBorder(const Frame& original, const Frame& enlarged, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const bool transparentIs0xFF);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_ENLARGER_H
