/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_DILATION_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_DILATION_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a dilation filter test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterDilation
{
	public:

		/**
		 * Tests the entire dilation filter.
		 * @param width The width of the test frame in pixel, with range [4, infinity)
		 * @param height The height of the test frame in pixel, with range [4, infinity)
		 * @param testDuration Number of seconds for each test
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the binary 8 bit dilation with a cross filter with diameter 3.
		 * @param width The width of the test frame in pixel, with range [2, infinity)
		 * @param height The height of the test frame in pixel, with range [2, infinity)
		 * @param testDuration Number of seconds for each test
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit4Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the binary 8 bit dilation filter with a 3x3 square filter.
		 * @param width The width of the test frame in pixel, with range [2, infinity)
		 * @param height The height of the test frame in pixel, with range [2, infinity)
		 * @param testDuration Number of seconds for each test
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit8Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		* Tests the binary 8 bit dilation filter with a 5x5 square filter.
		* @param width The width of the test frame in pixel, with range [4, infinity)
		* @param height The height of the test frame in pixel, with range [4, infinity)
		* @param testDuration Number of seconds for each test
		* @param worker The worker object
		* @return True, if succeeded
		*/
		static bool test8Bit24Neighbor(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the binary 8 bit dilation fitler for square filters and cross filters.
		 * @param width The width of the test frame in pixel, with range [4, infinity)
		 * @param height The height of the test frame in pixel, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test8Bit(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	private:

		/**
		 * Validates the binary 8 bit dilation filter for a cross kernel.
		 * @param mask The binary mask for which the result will be validated, must be valid
		 * @param target The filtered mask to validate, must be valid
		 * @param width The width of the mask in pixel, with range [2, infinity)
		 * @param height The height of the mask in pixel, with range [2, infinity)
		 * @param kernelSize The size of the dilation kernel in pixel, with range [3, infinity), must be odd
		 * @param maskValue The value of a mask pixel, all other pixels are treated as non-mask pixels, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validate8BitCrossKernel(const unsigned char* const mask, const unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const unsigned char maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements);


		/**
		 * Validates the binary 8 bit dilation filter for a square kernel.
		 * @param mask The binary mask for which the result will be validated, must be valid
		 * @param target The filtered mask to validate, must be valid
		 * @param width The width of the mask in pixel, with range [2, infinity)
		 * @param height The height of the mask in pixel, with range [2, infinity)
		 * @param kernelSize The size of the dilation kernel in pixel, with range [3, infinity), must be odd
		 * @param maskValue The value of a mask pixel, all other pixels are treated as non-mask pixels, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool validate8BitSquareKernel(const unsigned char* const mask, const unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int kernelSize, const unsigned char maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_DILATION_H
