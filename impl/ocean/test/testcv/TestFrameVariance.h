/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_VARIANCE_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_VARIANCE_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the frame variance functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameVariance
{
	public:

		/**
		 * Tests the frame variance functions
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the frame deviation function for a 1 channel 8 bit frame.<br>
		 * @param width The width of the source frame in pixel
		 * @param height The height of the source frame in pixel
		 * @param window The window of the sampling area
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements, either 'int8_t' or 'uint8_t'
		 */
		template <typename T>
		static bool testDeviation1Channel8Bit(const unsigned int width, const unsigned int height, const unsigned int window, const double testDuration);

		/**
		 * Tests the computation of per-channel mean, variance, and standard deviation of images
		 * This test will measure the performance for the specified width and
		 * height with and without padding.
		 * @param width The width of the source frame in pixel, range: [1, infinity)
		 * @param height The height of the source frame in pixel, range: [1, infinity)
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameStatistics(const unsigned width, const unsigned int height, const double testDuration);

		/**
		 * Tests the computation of per-channel mean, variance, and standard deviation of images
		 * This test will measure the performance for the specified width and
		 * height with and without padding.
		 * @param width Maximum width of the source frame in pixel, range: [1, infinity)
		 * @param height Maximum height of the source frame in pixel, range: [1, infinity)
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @tparam TElementType Type of the source pixel elements
		 * @tparam TElementType Type of the elements of the pixels of the input image
		 * @tparam TSummationType Type used for the internal computation of the pixel sums (should be at least as large as `TElementType`), cf. function description for details
		 * @tparam tChannels Number of channels of the source image, range: [1, infinity)
		 * @return True, if succeeded
		 */
		template <typename TElementType, typename TSummationType, typename TMultiplicationType, unsigned int tChannels>
		static bool testFrameStatistics(const unsigned width, const unsigned int height, const double testDuration);

		/**
		 * Validates the frame deviation function for a 1 channel 8 bit frame.<br>
		 * @param frame The frame for which the variance was determined, must be valid
		 * @param variance The variance to verify, must be valid
		 * @param window The window of the sampling area, with range [1, infinity), must be odd
		 * @return True, if succeeded
		 * @tparam T The data type of the frame's elements
		 * @tparam TVariance The data type of the variance's elements
		 */
		template <typename T, typename TVariance>
		static bool validateDeviation1Channel(const Frame& frame, const Frame& variance, const unsigned int window);

		/**
		 * Validation computation of per-channel mean, variance, and standard deviation of images.
		 * @param frame The frame for which the validation will be applied, must be valid
		 * @param width The width of the source frame in pixel, range: [1, infinity)
		 * @param height the height of the source frame in pixel, range: [1, infinity)
		 * @param paddingElements Number of padding elements, range: [0, infinity)
		 * @param testMean Mean results to be validated, must have @c tChannels elements, can be nullptr if not tested
		 * @param testVariance Variance results to be validated, must have @c tChannels elements, can be nullptr if not tested
		 * @param testStandardDeviation Standard deviation results to be validated, must have @c tChannels elements, can be nullptr if not tested
		 * @param maxErrorMean Resulting maximum error for mean over all channels
		 * @param maxErrorVariance Return the maximum error for variance over all channels
		 * @param maxErrorStandardDeviation Return the maximum error for standard deviation over all channels
		 * @return True, if succeeded
		 * @tparam TElementType Type of the elements of the source pixels
		 * @tparam tChannels Number of channels of the source image, range: [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static bool validateFrameStatistics(const TElementType* frame, const unsigned int width, const unsigned int height, const unsigned paddingElements, const double* testMean, const double* testVariance, const double* testStandardDeviation, double& maxErrorMean, double& maxErrorVariance, double& maxErrorStandardDeviation);
};

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_VARIANCE_H
