/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_FRAME_MIN_MAX_H
#define OCEAN_TEST_TESTCV_TEST_FRAME_MIN_MAX_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

/// Forward declaration
class RandomGenerator;

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameMinMax class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameMinMax
{
	public:

		/**
		 * Starts all test of the FrameMinMax class.
		 * @param width The width of the test image in pixel, with range [1, infinity)
		 * @param height The height of the test image in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the minimum value and location.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetermineMinValue(const double testDuration);

		/**
		 * Tests the function determining the maximum value and location.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetermineMaxValue(const double testDuration);

		/**
		 * Tests the function determining the minimal and maximum pixel value.
		 * @param width The width of the test image in pixel, with range [1, infinity)
		 * @param height The height of the test image in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDetermineMinMaxValues(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the minimal and maximum pixel value.
		 * @param width The width of the test image in pixel, with range [1, infinity)
		 * @param height The height of the test image in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool testDetermineMinMaxValues(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the function to count frame elements that are outside of a specified range of values
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCountElementsOutsideRange(const double testDuration);

		/**
		 * Tests the function determining the minimum value and location.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static bool testDetermineMinValue(const unsigned int width, const unsigned int height, const unsigned int paddingElements);

		/**
		 * Tests the function determining the minimal and maximum value.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static bool testDetermineMaxValue(const unsigned int width, const unsigned int height, const unsigned int paddingElements);

		/**
		 * Tests the function counting the elements outside of a specified range of values
		 * @param randomGenerator The random generator to be used
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static bool testCountElementsOutsideRange(RandomGenerator& randomGenerator, const uint32_t width, const uint32_t height, const uint32_t paddingElements);

	protected:

		/**
		 * Validates the minimal and maximal value which has been determined in a given image.
		 * @param frame The frame in which the minimal and maximal values have been determined, must be valid
		 * @param minValues The minimal values to validate, one for each channel, must be valid
		 * @param maxValues The maximal values to validate, one for each channel, must be valid
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel value
		 */
		template <typename T>
		static bool validateDetermineMinMaxValues(const Frame& frame, const T* const minValues, const T* const maxValues);
};

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_FRAME_MIN_MAX_H
