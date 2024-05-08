/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_FRAME_NORMALIZER_H
#define OCEAN_TEST_TESTCV_TEST_FRAME_NORMALIZER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameNormalizer class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameNormalizer
{
	public:

		/**
		 * Starts all tests of the FrameNormalizer class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the 1-channel normalizer to 8-bit function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type to be used for testing
		 */
		static bool testNormalizerToUint8(const double testDuration, Worker& worker);

		/**
		 * Tests the 1-channel normalizer to 8-bit function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type to be used for testing
		 * @tparam tExtremeValueRange True, to use an extreme value range for floating point frames
		 */
		template <typename T, bool tExtremeValueRange = false>
		static bool testNormalizerToUint8(const double testDuration, Worker& worker);

		/**
		 * Tests the normalization function to float images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testNormalizeToFloat(const double testDuration, Worker& worker);

		/**
		 * Tests the normalization function to float images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the normalized frame
		 * @tparam tChannels The number of channels of the source and normalized frames, range: [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static bool testNormalizeToFloat(const double testDuration, Worker& worker);

		/**
		 * Test the value range of the float normalizer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testValueRangeNormalizerToUint8(const double testDuration, Worker& worker);

		/**
		 * Test the value range of the float normalizer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam TFloat The float data type of the source frame, either 'float' or 'double'
		 */
		template <typename TFloat>
		static bool testValueRangeNormalizerToUint8(const double testDuration, Worker& worker);

	protected:

		/**
		 * Verifies the linear normalizer of arbitrary images to an 8 bit image.
		 * @param frame The input frame which will be normalized, must be valid
		 * @param normalized The normalized image to verify, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements of the input frame, in elements, with range [0, infinity)
		 * @param normalizedPaddingElements The number of padding elements of the normalized frame, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the source frame
		 */
		template <typename T>
		static bool verifyLinearNormalizedUint8(const T* const frame, const uint8_t* const normalized, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int normalizedPaddingElements);

		/**
		 * Verifies the linear normalizer arbitrary images to a float image
		 * @param frame The input frame which will be normalized, must be valid
		 * @param normalized The normalized image to verify, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param bias The bias values, must be valid if `scale` is valid and `nullptr` is `scale` is `nullptr`
		 * @param scale the scale values, must be valid if `bias` is valid and `nullptr` is `bias` is `nullptr`
		 * @param framePaddingElements The number of padding elements of the input frame, in elements, with range [0, infinity)
		 * @param normalizedPaddingElements The number of padding elements of the normalized frame, in elements, with range [0, infinity)
		 * @param maxAbsoluteError Returns the maximum absolute error that has been measured
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source frame
		 * @tparam TTarget The data type of the normalized frame
		 * @tparam tChannels The number of channels of the source and normalized frames, range: [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static bool verifyNormalizeToFloat(const TSource* frame, const TTarget* normalized, const unsigned int width, const unsigned height, const TTarget* bias, const TTarget* scale, const unsigned int framePaddingElements, const unsigned int normalizedPaddingElements, double& maxAbsoluteError);
};

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_FRAME_NORMALIZER_H
