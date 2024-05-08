/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_WHITE_POINT_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_WHITE_POINT_DETECTOR_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements a white point detection test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestWhitePointDetector
{
	public:

		/**
		 * Tests white point detection and correction functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker instance to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	private:

		/**
		 * Tests the histogram-based white point detection and correction functions.
		 * @param frame Test frame for white point detection, with RGB pixel format, must be valid
		 * @param whitePoint White point to use for generating the test data
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker instance to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testWhitepointDetectorHistogram(const Frame& frame, const VectorF3& whitePoint, const double testDuration, Worker& worker);

		/**
		 * Tests the gray points-based white point detection and correction functions.
		 * @param frame Test frame for white point detection, with RGB pixel format, must be valid
		 * @param whitePoint White point to use for generating the test data
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker instance to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testWhitepointDetectorGrayPoints(const Frame& frame, const VectorF3& whitePoint, const double testDuration, Worker& worker);

		/**
		 * Validates the specified detected white point by comparing it with a ground truth white point
		 * @param idealWhitePoint Ground truth white point
		 * @param detectedWhitePoint Detected white point that is compared with the ground truth
		 * @return True, if both white points are equal; otherwise, false is returned.
		 */
		static bool validateWhitePoint(const VectorF3& idealWhitePoint, const VectorF3& detectedWhitePoint);

		/**
		 * Validates a white point corrected frame by comparing its frame data with ground truth frame data
		 * @param idealFrame Ground truth frame, with RGB pixel format, must be valid
		 * @param correctedFrame white point corrected frame
		 * @return True, if the frame data of the both frames are (almost) equal; otherwise, false is returned.
		 */
		static bool validateWhitePointFrame(const Frame& idealFrame, const Frame& correctedFrame);

		/**
		 * Fills the specified frame with random pixels.
		 * @param frame The frame that is modified, must be valid
		 * @param randomAmount Amount of randomness, with range [0, 1]
		 */
		static void randomizeWhitePointFrame(Frame& frame, const float randomAmount = 0.5f);
};

}

}

}

}

#endif  // OCEAN_TEST_TESTCV_TESTADVANCED_TEST_WHITE_POINT_DETECTOR_H
