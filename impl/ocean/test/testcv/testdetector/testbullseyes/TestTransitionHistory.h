/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

/**
 * This class implements tests for the TransitionHistory class.
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestTransitionHistory
{
	public:

		/**
		 * Tests the TransitionHistory functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for TransitionHistory::history1(), history2(), history3(), and push()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testHistoryAndPush(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for TransitionHistory::reset()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testReset(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for TransitionHistory::isTransitionToBlack()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testIsTransitionToBlack(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for TransitionHistory::isTransitionToWhite()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testIsTransitionToWhite(const double testDuration, RandomGenerator& randomGenerator);
};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
