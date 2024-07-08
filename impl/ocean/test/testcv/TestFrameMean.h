/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_TEST_TESTCV_TEST_FRAME_MEAN_H
#define OCEAN_TEST_TESTCV_TEST_FRAME_MEAN_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrameMean class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameMean
{
	public:

		/**
		 * Starts all tests of the FrameMean class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests adding individual pixel values of a given source frame to a target frame if the corresponding mask pixels are valid.
		 * @param performanceWidth The image width that should be used to measure performance, range: [1, infinity)
		 * @param performanceHeight The image height that should be used to measure performance, range: [1, infinity)
		 * @param numberChannels The number of channels that the test images will have, range: [1, 4]
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 */
		static bool testAddToFrameIndividually(const unsigned int performanceWidth, const unsigned int performanceHeight, const unsigned numberChannels, const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the mean value for individual pixel formats.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testMeanValue(const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the mean value.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of channels of the test frame, with range, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel
		 * @tparam tChannels The number of frame channels
		 * @tparam TIntermediate The explicit data type of the internal sum values
		 */
		template <typename T, typename TMean, typename TIntermediate>
		static bool testMeanValue(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates adding individual pixel values of a given source frame to a target frame if the corresponding mask pixels are valid.
		 * The validation assumes that `target` and `denominators` had been set to all zeros before the computation of the test results.
		 * @param source The source frame that is used for the validation, must be valid and not have more than 4 channels and 8-bit unsigned int data type
		 * @param mask The mask frame that is used for the validation, must be valid and not have 1 channel and 8-bit unsigned int data type
		 * @param testTarget The target frame that is validated, must be valid and not have more than 4 channels and 32-bit unsigned int data type
		 * @param testDenominators The denominator frame that is validated, must be valid and not have 1 channel and 32-bit unsigned int data type
		 * @param nonMaskValue The value indicating invalid mask pixels, i.e. pixels that should not be processed by this function, all other values will be interpreted as valid, range: [0, 255]
		 * @return True if the validation was successful, otherwise false
		 */
		static bool validateAddToFrameIndividually(const Frame& source, const Frame& mask, const Frame& testTarget, const Frame& testDenominators, const uint8_t nonMaskValue);

		/**
		 * Validates the calculation of the mean value.
		 * @param frame The frame providing the data for which the mean must be verified, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels, width range [1, infinity)
		 * @param testMeanValues The already determined mean values to be validated, one for each channel, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel
		 * @tparam TMean The data type of the mean values
		 * @tparam TIntermediate The explicit data type of the internal sum values
		 */
		template <typename T, typename TMean, typename TIntermediate>
		static bool validateMeanValue(const T* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const TMean* testMeanValues, const unsigned int framePaddingElements);

		/**
		 * Determines the mean values based on the sum of all values and the number of values.
		 * Thus, this function mainly returns sum / size, while handling integer and floats slightly different.
		 * @param sum The sum of all values
		 * @param size The number of values
		 * @return The mean value
		 * @tparam T Data type of the sum and mean value
		 */
		template <typename T>
		static inline T meanValue(const T& sum, const size_t size);
};

template <typename T>
inline T TestFrameMean::meanValue(const T& sum, const size_t size)
{
	return (sum + T(size) / T(2)) / T(size);
}

template <>
inline float TestFrameMean::meanValue(const float& sum, const size_t size)
{
	return sum / float(size);
}

template <>
inline double TestFrameMean::meanValue(const double& sum, const size_t size)
{
	return sum / double(size);
}

}

}

}

#endif // OCEAN_TEST_TESTCV_TEST_FRAME_MEAN_H
