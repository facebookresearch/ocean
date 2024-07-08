/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/cv/detector/qrcodes/Utilities.h"

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
 * This class implements tests for the utility functions of the QR code library.
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestUtilities : protected CV::Detector::QRCodes::Utilities
{
	public:

		/**
		 * Tests the functions of the QR code utilities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for the check if a QR code is contained in a list of existing QR codes (without poses)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testContainsCode(const double testDuration);

		/**
		 * Tests the parsing of a Wi-Fi configuration stored as a string
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool stressTestParseWifiConfig(const double testDuration);

		/**
		 * Tests the parsing of a Wi-Fi configuration stored as a string
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testParseWifiConfig(const double testDuration);

		/**
		 * Tests the escaping of special characters in a string
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testEscapeSpecialCharacters(const double testDuration);

		/**
		 * Tests the unescaping of special characters in a string
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testUnescapeSpecialCharacters(const double testDuration);

	protected:

		/**
		 * Validates the escaping of special characters in a string
		 * @param string The string in which the special characters will be escaped, must be valid
		 * @param specialCharacters The list of characters that will be escaped in the string, must be valid
		 * @param testEscapedString The escaped string that will be tested for validity, must be valid
		 * @return True if the validation was successful, otherwise false
		 */
		static bool validateEscapeSpecialCharacters(const std::string& string, const std::string& specialCharacters, const std::string& testEscapedString);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Test
