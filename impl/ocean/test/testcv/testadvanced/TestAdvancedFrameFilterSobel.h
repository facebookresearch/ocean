/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SOBEL_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SOBEL_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements an advanced sobel frame filter test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedFrameFilterSobel
{
	public:

		/**
		 * Tests all sobel filter functions.
		 * @param width The width of the test frame in pixel, with range [3, infinity)
		 * @param height The height of the test frame in pixel, with range [3, infinity)
		 * @param depth Depth of the test frame in pixel, with range [3, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration, Worker& worker);

	protected:

		/**
		 * Tests the single pixel filtering.
		 * @param frame 3D test frame for fitering
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param depth Depth of the test frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSinglePixel(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration);

		/**
		 * Tests the maximum of horizontal and vertical filtering.
		 * @param frame 3D test frame for fitering
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param depth Depth of the test frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testFilterHorizontalVerticalMaximum8Bit(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const double testDuration, Worker& worker);

		/**
		 * Calculates the 3D sobel value for a given pixel.
		 * @param frame 3D test frame
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param depth Depth of the test frame in pixel
		 * @param x Horizontal pixel position
		 * @param y Vertical pixel position
		 * @param z Depth pixel position
		 * @return Sobel value
		 */
		static unsigned short sobel(const unsigned char* frame, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z);

		/**
		 * Tests the validation of the maximum horizontal and vertical 8 bit sobel filter.
		 * @param original Original gray scale image
		 * @param filtered Filtered gray scale image
		 * @param width The width of original and filtered image buffer in pixel
		 * @param height The height of original and filtered image buffer in pixel
		 * @param depth Depth of the test frame in pixel
		 * @return True, if succeeded
		 */
		static bool validationHorizontalVerticalMaximum8Bit(const unsigned char* original, const unsigned short* filtered, const unsigned int width, const unsigned int height, const unsigned int depth);

		/**
		 * Returns the pixel value of a given pixel position.
		 * @param frame 3D test frame for fitering
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param depth Depth of the test frame in pixel
		 * @param x Horizontal pixel position
		 * @param y Vertical pixel position
		 * @param z Depth pixel position
		 * @return Pixel value
		 */
		static inline unsigned char pixel(const unsigned char* const frame, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z);

		/**
		 * Returns the index of the 3D pixel for a given 3D point.
		 * @param width The width of the 3D frame in pixel
		 * @param height The height of the 3D frame in pixel
		 * @param depth Depth of the 3D frame in pixel
		 * @param x Horizontal pixel position
		 * @param y Vertical pixel position
		 * @param z Depth pixel position
		 * @return Corresponding pixel index
		 */
		static inline unsigned int index(const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z);
};

inline unsigned char TestAdvancedFrameFilterSobel::pixel(const unsigned char* const frame, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z)
{
	ocean_assert(frame != nullptr);

	return frame[index(width, height, depth, x, y, z)];
}

inline unsigned int TestAdvancedFrameFilterSobel::index(const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int x, const unsigned int y, const unsigned int z)
{
	ocean_assert_and_suppress_unused(x < width && y < height && z < depth, depth);

	return z * width * height + y * width + x;
}

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_FILTER_SOBEL_H
