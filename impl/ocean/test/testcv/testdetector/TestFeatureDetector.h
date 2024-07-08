/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FEATURE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FEATURE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements tests for the FeatureDetector class.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestFeatureDetector
{
	public:

		/**
		 * Invokes all tests.
		 * @param testFrame Optional explicit test frame to be used, can be invalid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const Frame& testFrame, const double testDuration, Worker& worker);

		/**
		 * Tests the Harris corner detection.
		 * This test is mainly a stress test.
		 * @param testFrame Optional explicit test frame to be used, can be invalid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDetermineHarrisPoints(const Frame& testFrame, const double testDuration, Worker& worker);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FEATURE_DETECTOR_H
