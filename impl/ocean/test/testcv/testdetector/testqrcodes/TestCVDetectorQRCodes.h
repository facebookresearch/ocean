/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

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
 * @ingroup testcvdetector
 * @defgroup testcvdetectorqrcodes Ocean Test CV Detector QR Codes Library
 * @{
 * The Ocean Test CV Detector QR Codes Library provides several functions to test the performance and validation of the QR code detection and generation functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestDetector::TestQRCodes Namespace of the CV Detector QR Codes Test library.<p>
 * The Namespace Ocean::Test::TestCV::TestDetector::TestQRCodes is used in the entire Ocean CV Detector QR Codes Test Library.
 */

// Defines OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT
		#define OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT
#endif

/**
 * Tests the entire Computer Vision Detector QR Codes library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcvdetector
 */
OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT bool testCVDetectorQRCodes(const double testDuration, Worker& worker, const std::string& testFunctions = "");

/**
 * Tests the entire Computer Vision Detector QR Codes library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvdetector
 */
OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT void testCVDetectorQRCodesAsynchronous(const double testDuration, const std::string& testFunctions = "");

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
