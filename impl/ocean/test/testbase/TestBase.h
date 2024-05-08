/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TESTBASE_H
#define META_OCEAN_TEST_TESTBASE_TESTBASE_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * @ingroup test
 * @defgroup testbase Ocean Test Base Library
 * @{
 * The Ocean Test Base Library provides several function to test the performance and validation of the basic ocean functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestBase Namespace of the Base Test library.<p>
 * The Namespace Ocean::Test::TestBase is used in the entire Ocean Base Test Library.
 */

// Defines OCEAN_TEST_BASE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_BASE_EXPORT
		#define OCEAN_TEST_BASE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_BASE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_BASE_EXPORT
#endif

/**
 * Tests the entire base library.
 * This function is intended for console applications.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testbase
 */
OCEAN_TEST_BASE_EXPORT bool testBase(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire base library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testbase
 */
OCEAN_TEST_BASE_EXPORT void testBaseAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TESTBASE_H
