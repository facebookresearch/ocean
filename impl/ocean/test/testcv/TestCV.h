/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTCV_H
#define META_OCEAN_TEST_TESTCV_TESTCV_H

#include "ocean/test/Test.h"

#include "ocean/cv/CV.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/SquareMatrix3.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * @ingroup test
 * @defgroup testcv Ocean Test CV Library
 * @{
 * The Ocean Test CV Library provides several function to test the performance and validation of the computer vision functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV Namespace of the CV Test library.<p>
 * The Namespace Ocean::Test::TestCV is used in the entire Ocean CV Test Library.
 */

// Defines OCEAN_TEST_CV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_EXPORT
		#define OCEAN_TEST_CV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_EXPORT
#endif

/**
 * Tests the entire Computer Vision library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object
 * @param testWidth Width of the test frame in pixel, with range [32, infinity)
 * @param testHeight Height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcv
 */
OCEAN_TEST_CV_EXPORT bool testCV(const double testDuration, Worker& worker, const unsigned int testWidth = 1280u, const unsigned int testHeight = 720u, const std::string& testFunctions = std::string());

/**
 * Tests the entire Computer Vision library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testWidth Width of the test frame in pixel, with range [32, infinity)
 * @param testHeight Height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcv
 */
OCEAN_TEST_CV_EXPORT void testCVAsynchron(const double testDuration, const unsigned int testWidth = 1280u, const unsigned int testHeight = 720u, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTCV_H
