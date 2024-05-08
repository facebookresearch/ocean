/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TESTCVSEGMENTATION_H
#define META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TESTCVSEGMENTATION_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

/**
 * @ingroup testcv
 * @defgroup testcvsegmentation Ocean Test CV Segmentation Library
 * @{
 * The Ocean Test CV Segmentation Library provides several function to test the performance and validation of the computer vision segmentation functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestSegmentation Namespace of the CV Segmentation Test library.<p>
 * The Namespace Ocean::Test::TestCV::TestSegmentation is used in the entire Ocean CV Segmentation Test Library.
 */

// Defines OCEAN_TEST_CV_SEGMENTATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_SEGMENTATION_EXPORT
		#define OCEAN_TEST_CV_SEGMENTATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_SEGMENTATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_SEGMENTATION_EXPORT
#endif

/**
 * Tests the entire Computer Vision Segmentation library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object
 * @param width The width of the test frame in pixel, with range [32, infinity)
 * @param height The height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcvsegmentation
 */
OCEAN_TEST_CV_SEGMENTATION_EXPORT bool testCVSegmentation(const double testDuration, Worker& worker, const unsigned int width = 1280u, const unsigned int height = 720u, const std::string& testFunctions = std::string());

/**
 * Tests the entire Computer Vision Segmentation library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param width The width of the test frame in pixel, with range [32, infinity)
 * @param height The height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvadvanced
 */
OCEAN_TEST_CV_SEGMENTATION_EXPORT void testCVSegmentationAsynchron(const double testDuration, const unsigned int width = 1280u, const unsigned int height = 720u, const std::string& testFunctions = std::string());

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TESTCVSEGMENTATION_H
