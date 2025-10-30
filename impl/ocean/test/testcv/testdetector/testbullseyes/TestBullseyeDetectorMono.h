/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/bullseyes/BullseyeDetectorMono.h"

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
 * This class implements tests for the BullseyeDetectorMono
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestBullseyeDetectorMono : protected CV::Detector::Bullseyes::BullseyeDetectorMono
{
	public:

		/**
		 * Tests the BullseyeDetectorMono functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Stress test for BullseyeDetectorMono::detectBullseyes()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @param randomGenerator A random generator that will be used to generate test data
		 * @return True, if succeeded
		 */
		static bool stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator);
};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
