/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TESTCVDETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TESTCVDETECTOR_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * @ingroup testcv
 * @defgroup testcvdetector Ocean Test CV Detector Library
 * @{
 * The Ocean Test CV Detector Library provides several functions to test the performance and validation of the computer vision detector functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestDetector Namespace of the CV Detector Test library.<p>
 * The Namespace Ocean::Test::TestCV::TestDetector is used in the entire Ocean CV Detector Test Library.
 */

// Defines OCEAN_TEST_CV_DETECTOR_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_DETECTOR_EXPORT
		#define OCEAN_TEST_CV_DETECTOR_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_DETECTOR_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_DETECTOR_EXPORT
#endif

/**
 * Tests the entire Computer Vision Detector library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testImageFilename The filename of the image to be used for testing
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcvdetector
 */
OCEAN_TEST_CV_DETECTOR_EXPORT bool testCVDetector(const double testDuration, Worker& worker, const std::string& testImageFilename, const std::string& testFunctions = std::string());

/**
 * Tests the entire Computer Vision Detector library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testImageFilename The filename of the image to be used for testing
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvdetector
 */
OCEAN_TEST_CV_DETECTOR_EXPORT void testCVDetectorAsynchron(const double testDuration, const std::string& testImageFilename, const std::string& testFunctions = std::string());

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TESTCVDETECTOR_H
