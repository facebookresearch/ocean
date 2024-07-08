/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FAST_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FAST_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/FASTFeature.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a FAST feature detector test.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestFASTDetector
{
	public:

		/**
		 * Tests the FAST feature detector.
		 * @param frame Test frame to be used for feature detection, must be valid with a dimension larger than 7x7
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const Frame& frame, const double testDuration, Worker& worker);

		/**
		 * Tests the FAST feature detector with standard strength determination.
		 * @param yFrame The frame which will be used for testing, with pixel format FORMAT_Y8, must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStandardStrength(const Frame& yFrame, const double testDuration, Worker& worker);

		/**
		 * Tests the FAST feature detector with precise strength determination.
		 * @param yFrame The frame which will be used for testing, with pixel format FORMAT_Y8, must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testPreciseStrength(const Frame& yFrame, const double testDuration, Worker& worker);

	private:

		/**
		 * Validates already detected FAST features.
		 * @param yFrame The frame in which the features have been detected, must be valid
		 * @param threshold FAST feature detection threshold
		 * @param features Already detected FAST features
		 * @return True, if succeeded
		 */
		static bool validate(const Frame& yFrame, const unsigned int threshold, const CV::Detector::FASTFeatures& features);

		/**
		 * Sorts two detector points.
		 * @param first First detector point
		 * @param second Second detector point
		 * @return True, if the first point should be located before the second point
		 */
		static bool sortPoints(const CV::Detector::FASTFeature& first, const CV::Detector::FASTFeature& second);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FAST_DETECTOR_H
