/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestQRCodes
{

/**
 * This class implements a test for the QR code detector.
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestQRCodeDetector2D : protected CV::Detector::QRCodes::QRCodeDetector2D
{
	public:

		/**
		 * Invokes all test for the QR code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test with random input data just ensuring that the detector does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);

		/**
		 * Test for the detection of QR codes using synthetic test data
		 * @param gaussianFilterSize Size of a Gaussian filter that is applied to the synthesized data; no filter is applied for the value 0, range: [0, infinity), must be an odd value
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodesSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
