/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FRAME_CHANGE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FRAME_CHANGE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements tests for frame change detection.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestFrameChangeDetector
{
	public:

		/**
		 * Tests all frame change detector functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the frame change detector consecutive frames with randomized inputs.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param nonStaticInput If true, the input will be randomized on each frame, almost certainly guaranteeing a change; otherwise, it will be static on each frame
		 * @param simulateDeviceMotion Whether to add random accelerometer and gyroscope readings and ensure that the detector handles them correctly
		 * @param forcedKeyframes Whether to require keyframes at fixed intervals
		 * @param worker The worker object; to test single- and multi-core performance individual trials may or may not use this
		 * @return True, if succeeded
		 */
		static bool testInput(const double testDuration, bool nonStaticInput, bool simulateDeviceMotion, bool forcedKeyframes, Worker& worker);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FRAME_CHANGE_DETECTOR_H
