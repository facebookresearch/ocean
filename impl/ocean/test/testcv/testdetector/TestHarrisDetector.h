/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HARRIS_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HARRIS_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/HarrisCorner.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a Harris corner detector test.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestHarrisDetector
{
	public:

		/**
		 * Tests the Harris corner detector.
		 * @param frame Test frame to be used for feature detection, invalid to use a random image
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const Frame& frame, const double testDuration, Worker& worker);

		/**
		 * Tests the threshold function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testThreshold(const double testDuration);

		/**
		 * Tests the Harris corner detector with pixel accuracy.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testPixelAccuracy(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the Harris corner detector with pixel accuracy for a frame with one rectangle.
		 * @param width The width of the frame in pixel, with range [20, infinity)
		 * @param height The height of the frame in pixel, with range [20, infinity)
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testPixelAccuracyCorners(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the Harris corner detector with sub-pixel accuracy.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testSubPixelAccuracy(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the corner detection function in a sub-frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testSubFrameDetection(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the corner detection function in a checkerboard image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testCheckerboardDetection(const double testDuration, Worker& worker);

		/**
		 * Tests the Harris response for a single pixel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testHarrisVotePixel(const double testDuration, Worker& worker);

		/**
		 * Tests the Harris response for an entire frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testHarrisVoteFrame(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

	private:

		/**
		 * Validates the Harris corner detector with pixel accuracy.
		 * @param yFrame Original gray scale frame to be used for feature detection, must be valid
		 * @param threshold Harris feature detection threshold
		 * @param features The features for which the accuracy will be validated
		 * @return True, if succeeded
		 */
		static bool validatePixelAccuracy(const Frame& yFrame, const unsigned int threshold, const CV::Detector::HarrisCorners& features);

		/**
		 * Determines the horizontal gradient (Sobel response) at a given position.
		 * @param yFrame The frame in which the gradient will be determined, must be valid
		 * @param x Horizontal position in pixel, with range [1, width - 1)
		 * @param y Vertical position in pixel, with range [1, height - 1)
		 * @return Resulting horizontal response
		 * @tparam tRoundedDivision True, to use a rounded division for integers; False, to use a normal division
		 */
		template <bool tRoundedDivision>
		static int horizontalGradient(const Frame& yFrame, const unsigned int x, const unsigned int y);

		/**
		 * Determines the vertical gradient (Sobel response) at a given position.
		 * @param yFrame The frame in which the gradient will be determined, must be valid
		 * @param x Horizontal position in pixel, with range [1, width - 1)
		 * @param y Vertical position in pixel, with range [1, height - 1)
		 * @return Resulting vertical response
		 * @tparam tRoundedDivision True, to use a rounded division for integers; False, to use a normal division
		 */
		template <bool tRoundedDivision>
		static int verticalGradient(const Frame& yFrame, const unsigned int x, const unsigned int y);

		/**
		 * Determines the Harris vote for a given point in a 3x3 neighborhood.
		 * @param yFrame The frame in which the Harris corner vote is determined, must be valid
		 * @param x Horizontal position with range [2, width - 2)
		 * @param y Vertical position with range [2, height - 2)
		 * @return Resulting Harris vote
		 * @tparam tRoundedDivision True, to use a rounded division for integers; False, to use a normal division
		 */
		template <bool tRoundedDivision>
		static int harrisVote3x3(const Frame& yFrame, const unsigned int x, const unsigned int y);

		/**
		 * Sorts two Harris corner objects.
		 * @param a First object
		 * @param b Second object
		 * @return True, if the first object is lesser than the second object
		 */
		static bool sortHarris(const CV::Detector::HarrisCorner& a, const CV::Detector::HarrisCorner& b);

		/**
		 * Performs a rounded division of integer values.
		 * @param value The nominator, with range (-infinity, infinity)
		 * @param denominator The denominator, with range [1, infinity)
		 * @return The rounded result of the division
		 */
		static inline int roundedDivision(const int value, const unsigned int denominator);
};

inline int TestHarrisDetector::roundedDivision(const int value, const unsigned int denominator)
{
	ocean_assert(denominator != 0);

	if (value >= 0)
	{
		return (value + int(denominator / 2u)) / int(denominator);
	}
	else
	{
		return (value - int(denominator / 2u)) / int(denominator);
	}
}

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_HARRIS_DETECTOR_H
