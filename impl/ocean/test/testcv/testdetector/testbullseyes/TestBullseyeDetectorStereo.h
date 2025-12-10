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
 * This class implements tests for the BullseyeDetectorStereo
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestBullseyeDetectorStereo : protected CV::Detector::Bullseyes::BullseyeDetectorStereo
{
	public:

		/**
		 * Helper struct containing a randomized stereo camera setup for testing.
		 */
		struct StereoTestSetup
		{
			/// Camera A (left camera)
			SharedAnyCamera cameraA;

			/// Camera B (right camera)
			SharedAnyCamera cameraB;

			/// Transformation from device to world coordinates
			HomogenousMatrix4 world_T_device;

			/// Transformation from camera A to world coordinates
			HomogenousMatrix4 world_T_cameraA;

			/// Transformation from camera B to world coordinates
			HomogenousMatrix4 world_T_cameraB;
		};

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

	protected:

		/**
		 * Creates a randomized stereo camera setup for testing.
		 * @param randomGenerator The random generator to use
		 * @return A StereoTestSetup with randomized cameras and poses
		 */
		static StereoTestSetup createRandomStereoSetup(RandomGenerator& randomGenerator);

		/**
		 * Creates a random 3D world point that is visible in both cameras of the stereo setup.
		 * @param randomGenerator The random generator to use
		 * @param setup The stereo test setup containing cameras and poses
		 * @param worldPoint The resulting world point (output)
		 * @param imagePointA The projection of the world point in camera A (output)
		 * @param imagePointB The projection of the world point in camera B (output)
		 * @return True if a valid visible point was generated, false otherwise
		 */
		static bool createRandomVisibleWorldPoint(RandomGenerator& randomGenerator, const StereoTestSetup& setup, Vector3& worldPoint, Vector2& imagePointA, Vector2& imagePointB);

		/**
		 * Creates a bullseye at a given position with random diameter and threshold.
		 * @param randomGenerator The random generator to use
		 * @param position The 2D position for the bullseye
		 * @return A Bullseye at the specified position with random parameters
		 */
		static CV::Detector::Bullseyes::Bullseye createRandomBullseyeAtPosition(RandomGenerator& randomGenerator, const Vector2& position);
};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
