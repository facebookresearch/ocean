/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/bullseyes/Utilities.h"

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
 * This class implements tests for the Utilities class.
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestUtilities
{
	protected:

		/// Alias for the Utilities class.
		using Utilities = CV::Detector::Bullseyes::Utilities;

		/// Alias for the Bullseye class.
		using Bullseye = CV::Detector::Bullseyes::Bullseye;

	public:

		/**
		 * Tests the Utilities functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for Utilities::createBullseyeImage()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testCreateBullseyeImage(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Utilities::drawBullseyeWithOffset()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testDrawBullseyeWithOffset(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Utilities::drawBullseye()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testDrawBullseye(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Test for Utilities::drawBullseyes()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testDrawBullseyes(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Creates a random valid bullseye with random position, radius, and gray threshold.
		 * This is a shared helper function used by multiple test classes.
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
