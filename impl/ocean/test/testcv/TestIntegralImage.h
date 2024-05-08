/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_INTEGRAL_IMAGE_H
#define META_OCEAN_TEST_TESTCV_TEST_INTEGRAL_IMAGE_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements an integral image tester.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestIntegralImage
{
	public:

		/**
		 * Tests all integral image functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the standard integral image function without any border.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the standard integral image function without any border.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the lined integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the comfort function for lined integral images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinedIntegralImageComfort(const double testDuration);

		/**
		 * Tests the lined integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testLinedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the lined squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the lined squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testLinedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the joined lined integral and squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinedIntegralImageAndSquaredJoined(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the joined lined integral and squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegralAndSquared The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
		static bool testLinedIntegralImageAndSquaredJoined(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the separate lined integral and squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLinedIntegralImageAndSquaredSeparate(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the separate lined integral and squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam TIntegralSquared The data type of each squared integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
		static bool testLinedIntegralImageAndSquaredSeparate(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBorderedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the comfort function for bordered integral images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBorderedIntegralImageComfort(const double testDuration);

		/**
		 * Tests the bordered integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testBorderedIntegralImage(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBorderedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered squared integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testBorderedIntegralImageSquared(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered mirrored integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBorderedIntegralImageMirror(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered mirrored integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testBorderedIntegralImageMirror(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered squared mirrored integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBorderedIntegralImageSquaredMirror(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered squared mirrored integral image function.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool testBorderedIntegralImageSquaredMirror(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bordered squared mirrored integral image function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVarianceCalculation(const double testDuration);

		/**
		 * Tests the variance calculation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam TIntegralSquared The data type of each integral element, e.g., 'unsigned long long'
		 * @tparam TVariance The data type of the variance, e.g., `float` or `double`
		 */
		template <typename T, typename TIntegral, typename TIntegralSquared, typename TVariance>
		static bool testVarianceCalculation(const double testDuration);

		/**
		 * Tests the bordered squared mirrored integral image function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVarianceCalculationTwoRegions(const double testDuration);

		/**
		 * Tests the variance calculation function for two joined regions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam TIntegralSquared The data type of each integral element, e.g., 'unsigned long long'
		 * @tparam TVariance The data type of the variance, e.g., `float` or `double`
		 */
		template <typename T, typename TIntegral, typename TIntegralSquared, typename TVariance>
		static bool testVarianceCalculationTwoRegions(const double testDuration);

	private:

		/**
		 * Validates the integral image without border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of source frame (and integral image) in pixel, with range [1, infinity)
		 * @param height The height of source frame (and integral image) in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each integral row, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 */
		template <typename T, typename TIntegral>
		static bool validateIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks = 20u);

		/**
		 * Validates the integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param channels The number of channels the given image (and integral image has), with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 */
		template <typename T, typename TIntegral>
		static bool validateBorderedIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Validates the integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool validateBorderedIntegralImage(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Validates the squared integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool validateBorderedIntegralImageSquared(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks = 20u);

		/**
		 * Validates the joined integral and squared integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integralAndSquared The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralAndSquaredPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegralAndSquared The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
		static bool validateBorderedIntegralImageAndSquaredJoined(const T* source, const TIntegralAndSquared* integralAndSquared, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralAndSquaredPaddingElements, const unsigned int validationChecks = 20u);

		/**
		 * Validates the separate integral and squared integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param integralSquared The integral squared image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @param integralSquaredPaddingElements Optional number of padding elements at the end of each row of the integral squared image, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam TIntegralSquared The data type of each integral squared element, e.g., 'unsigned long long'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
		static bool validateBorderedIntegralImageAndSquaredSeparate(const T* source, const TIntegral* integral, const TIntegralSquared* integralSquared, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int integralSquaredPaddingElements, const unsigned int validationChecks = 20u);

		/**
		 * Validates the mirrored integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool validateBorderedIntegralImageMirror(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks = 20u);

		/**
		 * Validates the squared mirrored integral image with border.
		 * @param source The source frame from which the integral image has been created, must be valid
		 * @param integral The integral image to be validated, must be valid
		 * @param width The width of original image in pixel, with range [1, infinity)
		 * @param height The height of original image in pixel, with range [1, infinity)
		 * @param border The border of the integral image, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each row of the source image, with range [0, infinity)
		 * @param integralPaddingElements Optional number of padding elements at the end of each row of the integral image, with range [0, infinity)
		 * @param validationChecks The number of random validation checks to be done, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each source elements, e.g,. 'unsigned char'
		 * @tparam TIntegral The data type of each integral element, e.g., 'unsigned int'
		 * @tparam tChannels The number of channels the given image (and integral image has), with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static bool validateBorderedIntegralImageSquaredMirror(const T* source, const TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int validationChecks = 20u);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_INTEGRAL_IMAGE_H
