/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTACKING_TEST_TRACKING_H
#define META_OCEAN_TEST_TESTTACKING_TEST_TRACKING_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * @ingroup test
 * @defgroup testtracking Ocean Test Tracking Library
 * @{
 * The Ocean Test Tracking Library implements tests for Ocean's Tracking library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestTracking Namespace of the Tracking Test library.<p>
 * The Namespace Ocean::Test::TestTracking is used in the entire Ocean Tracking Test Library.
 */

// Defines OCEAN_TEST_TRACKING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_TRACKING_EXPORT
		#define OCEAN_TEST_TRACKING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_TRACKING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_TRACKING_EXPORT
#endif

/**
 * Tests the entire tracking library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testtracking
 */
OCEAN_TEST_TRACKING_EXPORT bool testTracking(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire tracking library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * This function in intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testtracking
 */
OCEAN_TEST_TRACKING_EXPORT void testTrackingAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTTACKING_TEST_TRACKING_H
