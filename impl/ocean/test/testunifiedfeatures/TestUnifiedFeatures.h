// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestUnifiedFeatures
{

/**
 * @ingroup test
 * @defgroup testunifiedfeatures Ocean Test Unified Features Library
 * @{
 * The Ocean Test Unified Features Library implements tests for Ocean's Unified Features library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestUnifiedFeatures Namespace of the Unified Features Test library.<p>
 * The Namespace Ocean::Test::TestUnifiedFeatures is used in the entire Ocean Unified Features Test Library.
 */

// Defines OCEAN_TEST_UNIFIEDFEATURES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_UNIFIEDFEATURES_EXPORT
		#define OCEAN_TEST_UNIFIEDFEATURES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_UNIFIEDFEATURES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_UNIFIEDFEATURES_EXPORT
#endif

/**
 * Tests the entire unified features library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testunifiedfeatures
 */
OCEAN_TEST_UNIFIEDFEATURES_EXPORT bool test(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire unified features library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * This function in intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testunifiedfeatures
 */
OCEAN_TEST_UNIFIEDFEATURES_EXPORT void testAsynchron(const double testDuration, const std::string& testFunctions = std::string());

} // namespace TestUnifiedFeatures

} // namespace Test

} // namespace Ocean
