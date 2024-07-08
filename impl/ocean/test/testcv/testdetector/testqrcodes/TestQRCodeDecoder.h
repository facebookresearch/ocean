/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

namespace Ocean
{

/// Forward declaration
class RandomGenerator;

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestQRCodes
{

/**
 * This class implements tests for the QR code features
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestQRCodeDecoder
{
	public:

		/**
		 * Tests the QR code functions.
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);


		/**
		 * Tests encoding (generation) of QR codes
		 * @param testDuration The duration in seconds for which this test will be run, range: (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testQRCodeDecoding(const double testDuration);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespce Test

} // namespace Ocean
