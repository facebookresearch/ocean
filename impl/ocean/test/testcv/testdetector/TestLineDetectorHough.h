/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_HOUGH_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_HOUGH_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/LineDetectorHough.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements houg-transformation-based line detector tests.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestLineDetectorHough
{
	public:

		/**
		 * Tests the line detector functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the accumulator joining function.
		 * @param width The width of the test frame, with range [7, infinity)
		 * @param height The height of the test frame, with range [7, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testAccumulatorJoin(const unsigned int width, const unsigned int height, Worker& worker);

		/**
		 * Tests the line detector on a random frame without any distinct edges.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testLineDetectorRandomFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the line detector on an artificial frame with strong edges.
		 * @param width The width of the test frame, with range [7, infinity)
		 * @param height The height of the test frame, with range [7, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testLineDetectorArtificialFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	private:

		/**
		 * Validates the mirrored smooth function of the accumulator array.
		 * @param original The original buffer which has been filtered
		 * @param smoothAccumulator Filtered buffer to be validated
		 * @param width The width of the accumulator buffer in pixel, with range [3, infinity)
		 * @param height The height of the accumulator buffer in pixel, with range [3, infinity)
		 * @return True, if succeeded
		 */
		static bool validateSmooth(const unsigned int* original, const unsigned int* smoothAccumulator, const unsigned int width, const unsigned int height);

		/**
		 * Validates joined accumulator arrays.
		 * @param original The original accumulator objects
		 * @param joined The joined accumulator object
		 * @param accumulators Number of accumulators to be joined
		 * @return True, if succeeded
		 */
		static bool validateJoin(const CV::Detector::LineDetectorHough::Accumulator* original, const CV::Detector::LineDetectorHough::Accumulator* joined, const unsigned int accumulators);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_DETECTOR_HOUGH_H
