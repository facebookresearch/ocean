/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TESTCVSYNTHESIS_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TESTCVSYNTHESIS_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * @ingroup testcv
 * @defgroup testcvsynthesis Ocean Test CV Synthesis Library
 * @{
 * The Ocean Test CV Synthesis Library provides several function to test the performance and validation of the computer vision synthesis functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestSynthesis Namespace of the CV Synthesis Test library.<p>
 * The Namespace Ocean::Test::TestCV::TestSynthesis is used in the entire Ocean CV Synthesis Test Library.
 */

// Defines OCEAN_TEST_CV_SYNTHESIS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_SYNTHESIS_EXPORT
		#define OCEAN_TEST_CV_SYNTHESIS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_SYNTHESIS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_SYNTHESIS_EXPORT
#endif

/**
 * Tests the entire computer vision synthesis library.
 * @param testDuration Number of seconds for each test
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testWidth Width of the test frame in pixel, with range [32, infinity)
 * @param testHeight Height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcvsynthesis
 */
OCEAN_TEST_CV_SYNTHESIS_EXPORT bool testCVSynthesis(const double testDuration, Worker& worker, const unsigned int testWidth = 1280u, const unsigned int testHeight = 720u, const std::string& testFunctions = std::string());

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TESTCVSYNTHESIS_H
