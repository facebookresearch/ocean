/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/bullseyes/BullseyeDetectorStereo.h"

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
 * This class implements tests for the BullseyeDetectorStereo
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestBullseyeDetectorStereo : protected CV::Detector::Bullseyes::BullseyeDetectorStereo
{
	public:

		/**
		 * Tests the BullseyeDetectorStereo functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Stress test for BullseyeDetectorStereo::detectBullseyes()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the Parameters class.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testParameters(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the invalidMatchingCost() static function.
		 * @return True, if succeeded
		 */
		static bool testInvalidMatchingCost();

		/**
		 * Tests the Candidate class.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testCandidate(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the triangulateBullseye() static function.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testTriangulateBullseye(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the extractBullseyeCandidates() protected function.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testExtractBullseyeCandidates(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the extractBullseyes() protected function.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testExtractBullseyes(const double testDuration, RandomGenerator& randomGenerator);

		/**
		 * Tests the computeCostMatrix() protected function.
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool testComputeCostMatrix(const double testDuration, RandomGenerator& randomGenerator);
};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
