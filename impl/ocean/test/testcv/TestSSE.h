/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_SSE_H
#define META_OCEAN_TEST_TESTCV_TEST_SSE_H

#include "ocean/test/testcv/TestCV.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Computer Vision SSE test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestSSE
{
	public:

		/**
		 * Tests the entire SSE class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 1 channel 8 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging1Channel8Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 1 channel 8 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging1Channel32Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 2 channel 16 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging2Channel16Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 2 channel 66 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging2Channel64Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 3 channel 24 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging3Channel24Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 3 channel 96 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging3Channel96Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 4 channel 32 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging4Channel32Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 4 channel 32 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging4Channel128Bit2x2(const double testDuration);

		/*
		 * Tests the binary SSE averaging functions for 1 channel 8 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testBinaryAveraging1Channel8Bit2x2(const double testDuration);

		/*
		 * Tests the SSE averaging functions for 1 channel 8 bit.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveraging1Channel8Bit3x3(const double testDuration);

		/**
		 * De-interleave test for 8 bit 3 channels data with 15 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeInterleave3Channel8Bit15Elements(const double testDuration);

		/**
		 * De-interleave test for 8 bit 3 channels data with 24 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeInterleave3Channel8Bit24Elements(const double testDuration);

		/**
		 * De-interleave test for 8 bit 3 channels data with 48 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeInterleave3Channel8Bit48Elements(const double testDuration);

		/**
		 * De-interleave test for 8 bit 3 channels data with 45 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDeInterleave3Channel8Bit45Elements(const double testDuration);

		/**
		 * Interleave test for 8 bit 3 channels data with 48 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterleave3Channel8Bit48Elements(const double testDuration);

		/**
		 * Reverse channel order test for 8 bit 2 channels data with 32 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReverseChannelOrder2Channel8Bit32Elements(const double testDuration);

		/**
		 * Reverse channel order test for 8 bit 3 channels data with 48 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReverseChannelOrder3Channel8Bit48Elements(const double testDuration);

		/**
		 * Reverse channel order test for 8 bit 4 channels data with 64 elements.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReverseChannelOrder4Channel8Bit64Elements(const double testDuration);

		/**
		 * Reverse channel order and swap test for 8 bit 3 channels data with 48 elements.
		 * @return True, if succeeded
		 */
		static bool testSwapReversedChannelOrder3Channel8Bit48Elements();

		/**
		 * Reverse element order test for 48 elements with 8 bit per element.
		 * @return True, if succeeded
		 */
		static bool testReverseElements8Bit48Elements();

		/**
		 * Reverse element order and swap test for 48 elements with 8 bit per element.
		 * @return True, if succeeded
		 */
		static bool testSwapReversedElements8Bit48Elements();

		/**
		 * Sum of 1x16 elements of an interleaved image with 1 channels and 8 bit per channel.
		 * @return True, if succeeded
		 */
		static bool testSumInterleave1Channel8Bit16Elements();

		/**
		 * Sum of 1x15 elements of an interleaved image with 1 channels and 8 bit per channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSumInterleave1Channel8Bit15Elements(const double testDuration);

		/**
		 * Sum of 3x16 elements of an interleaved image with 3 channels and 8 bit per channel.
		 * @return True, if succeeded
		 */
		static bool testSumInterleave3Channel8Bit48Elements();

		/**
		 * Sum of 3x15 elements of an interleaved image with 3 channels and 8 bit per channel.
		 * @return True, if succeeded
		 */
		static bool testSumInterleave3Channel8Bit45Elements();

		/**
		 * Tests the interpolation function for 3 channels and 24 bit per pixel image data.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolation1Channel8Bit15Elements(const double testDuration);

		/**
		 * Tests the interpolation function for 3 channels and 24 bit per pixel image data.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolation3Channel24Bit12Elements(const double testDuration);

		/**
		 * Tests the function adding an offset for 16 bit signed integer right shifts to perform a division by 2.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddOffsetBeforeRightShiftDivisionByTwoSigned16Bit(const double testDuration);

		/**
		 * Tests the function adding an offset for 16 bit signed integer right shifts to perform a division.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddOffsetBeforeRightShiftDivisionSigned16Bit(const double testDuration);

		/**
		 * Tests the function adding an offset for 32 bit signed integer right shifts to perform a division by 2.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddOffsetBeforeRightShiftDivisionByTwoSigned32Bit(const double testDuration);

		/**
		 * Tests the function adding an offset for 32 bit signed integer right shifts to perform a division.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddOffsetBeforeRightShiftDivisionSigned32Bit(const double testDuration);

		/**
		 * Tests the function multiplying eight 16 bit integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultiplyInt8x16ToInt32x8(const double testDuration);

		/**
		 * Tests the function multiplying and accumulating eight 16 bit integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultiplyInt8x16ToInt32x8AndAccumulate(const double testDuration);

	private:

		/**
		 * Average of 2x elements pixel of a gray scale image with 8 bit using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {8, 16, 32}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements1Channel8Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x elements pixel of a binary image with 8 bit using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {16}
		 * @param sumThreshold Minimal sum threshold of four pixels to result in a pixel with value 255
		 * @return True, if all result are valid
		 */
		static bool testAverageElementsBinary1Channel8Bit2x2(const double testDuration, const unsigned int elements, const unsigned int sumThreshold = 640u);

		/**
		 * Average of 2x elements pixel of a gray scale image with 32 bit using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {8}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements1Channel32Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 3x elements pixel of a gray scale image with 8 bit using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {30}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements1Channel8Bit3x3(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/2' pixel of a gray scale image with alpha channel and 8 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {8}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements2Channel16Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/2' pixel of a gray scale image with alpha channel and 32 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {8, 16, 32}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements2Channel64Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/3' pixel of a 3 channel image and 8 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {12, 24}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements3Channel24Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/3' pixel of a 3 channel image and 32 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {6}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements3Channel96Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/4' pixel of a 4 channel image and 8 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {16, 32}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements4Channel32Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x 'elements/4' pixel of a 4 channel image and 32 bit per channel using SSE function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param elements element size of SSE function {8}
		 * @return True, if all result are valid
		 */
		static bool testAverageElements4Channel128Bit2x2(const double testDuration, const unsigned int elements);

		/**
		 * Average of 2x2 pixel block of a gray scale image with 8 bit.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even gray image lines
		 * @param imageLine1Buffer8Bit buffer of odd gray image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements1Channel8Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine);

		/**
		 * Average of 2x2 pixel block of an binary image with 8 bit.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even binary image lines
		 * @param imageLine1Buffer8Bit buffer of odd binary image lines
		 * @param averageLine resulting average image line
		 * @param sumThreshold Minimal sum threshold of four pixels to result in a pixel with value 255
		 * @return True, if succeeded
		 */
		static bool referenceAverageElementsBinary1Channel8Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine, const unsigned int sumThreshold = 766u);

		/**
		 * Average of 3x3 pixel block of a gray scale image with 8 bit.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even gray image lines
		 * @param imageLine1Buffer8Bit buffer of odd gray image lines
		 * @param imageLine2Buffer8Bit buffer of odd gray image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements1Channel8Bit3x3(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, const uint8_t* const imageLine2Buffer8Bit, uint8_t* const averageLine);

		/**
		 * Average of 2x2 pixel block of a gray scale image with 32 bit.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer32Bit buffer of even gray image lines
		 * @param imageLine1Buffer32Bit buffer of odd gray image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements1Channel32Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine);

		/**
		 * Average of 2x2 pixel block of a gray scale image with alpha channel and 8 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even image lines
		 * @param imageLine1Buffer8Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements2Channel16Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine);

		/**
		 * Average of 2x2 pixel block of a gray scale image with alpha channel and 32 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer32Bit buffer of even image lines
		 * @param imageLine1Buffer32Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements2Channel64Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine);

		/**
		 * Average of 2x2 pixel block of an 3 channel image and 8 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even image lines
		 * @param imageLine1Buffer8Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements3Channel24Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine);

		/**
		 * Average of 2x2 pixel block of an 3 channel image and 32 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer32Bit buffer of even image lines
		 * @param imageLine1Buffer32Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements3Channel96Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine);

		/**
		 * Average of 2x2 pixel block of an 4 channel image and 8 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer8Bit buffer of even image lines
		 * @param imageLine1Buffer8Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements4Channel32Bit2x2(const unsigned int elements, const uint8_t* const imageLine0Buffer8Bit, const uint8_t* const imageLine1Buffer8Bit, uint8_t* const averageLine);

		/**
		 * Average of 2x2 pixel block of an 4 channel image and 32 bit per channel.
		 * @param elements length of averageLine buffer
		 * @param imageLine0Buffer32Bit buffer of even image lines
		 * @param imageLine1Buffer32Bit buffer of odd image lines
		 * @param averageLine resulting average image line
		 * @return True, if succeeded
		 */
		static bool referenceAverageElements4Channel128Bit2x2(const unsigned int elements, const float* const imageLine0Buffer32Bit, const float* const imageLine1Buffer32Bit, float* const averageLine);
};

}

}

}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_TEST_TESTCV_TEST_SSE_H
