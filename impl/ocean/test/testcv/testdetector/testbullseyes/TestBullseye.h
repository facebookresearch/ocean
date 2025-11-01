/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/bullseyes/Bullseye.h"

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
 * This class implements tests for the Bullseye class
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestBullseye
{
	protected:

		/// Alias for the Bullseye class.
		using Bullseye = CV::Detector::Bullseyes::Bullseye;

	public:

		/**
		 * Tests the Bullseye functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for Bullseye::isValid()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testBullseyeIsValid(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Bullseye constructor
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testBullseyeConstructor(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Bullseye::position()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testBullseyePosition(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Bullseye::radius()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testBullseyeRadius(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Bullseye::grayThreshold()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testBullseyeGrayThreshold(const double testDuration, RandomGenerator& randomGenerator);

	protected:

		/**
		 * Creates a random valid bullseye with random position, radius, and gray threshold
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return A valid bullseye with random parameters
		 */
		static Bullseye createRandomValidBullseye(RandomGenerator& randomGenerator);
};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
