/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"

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
 * This class implements tests for the Micro QR code features
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestMicroQRCodeDecoder
{
	protected:

		/**
		 * Definition of a struct holding the parameters describing a portion of the Micro QR code encoding space
		 */
		struct EncodingParameters
		{
			CV::Detector::QRCodes::MicroQRCode::EncodingMode mode;
			unsigned int minSize;
			unsigned int maxSize;
			CV::Detector::QRCodes::MicroQRCode::ErrorCorrectionCapacity eccMin = CV::Detector::QRCodes::MicroQRCode::ECC_DETECTION_ONLY;
		};

	public:

		/**
		 * Tests the Micro QR code functions.
		 * @param testDuration Number of seconds for each test, range: (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);


		/**
		 * Tests encoding (generation) of Micro QR codes
		 * @param testDuration The duration in seconds for which this test will be run, range: (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMicroQRCodeDecoding(const double testDuration);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespce Test

} // namespace Ocean
