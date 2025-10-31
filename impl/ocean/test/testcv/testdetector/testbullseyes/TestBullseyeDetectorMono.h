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
		 * Tests the default constructor of Parameters.
		 * @return True, if succeeded
		 */
		static bool testParametersConstructor();

		/**
		 * Tests the Parameters::isValid() function.
		 * @return True, if succeeded
		 */
		static bool testParametersIsValid();

		/**
		 * Tests the Parameters::framePyramidPixelThreshold() function.
		 * @return True, if succeeded
		 */
		static bool testParametersFramePyramidPixelThreshold();

		/**
		 * Tests the Parameters::setFramePyramidPixelThreshold() function.
		 * @return True, if succeeded
		 */
		static bool testParametersSetFramePyramidPixelThreshold();

		/**
		 * Tests the Parameters::framePyramidLayers() function.
		 * @return True, if succeeded
		 */
		static bool testParametersFramePyramidLayers();

		/**
		 * Tests the Parameters::setFramePyramidLayers() function.
		 * @return True, if succeeded
		 */
		static bool testParametersSetFramePyramidLayers();

		/**
		 * Tests the Parameters::useAdaptiveRowSpacing() function.
		 * @return True, if succeeded
		 */
		static bool testParametersUseAdaptiveRowSpacing();

		/**
		 * Tests the Parameters::setUseAdaptiveRowSpacing() function.
		 * @return True, if succeeded
		 */
		static bool testParametersSetUseAdaptiveRowSpacing();

		/**
		 * Tests the Parameters::defaultParameters() function.
		 * @return True, if succeeded
		 */
		static bool testParametersDefaultParameters();

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
