// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTPLATFORM_TEST_PLATFORM_H
#define META_OCEAN_TEST_TESTPLATFORM_TEST_PLATFORM_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Test
{

namespace TestPlatform
{

/**
 * @ingroup test
 * @defgroup testplatform Ocean Test Platform Library
 * @{
 * The Ocean Test Platform Library implemenets/provides several tests for Ocean's individual platform libraries.
 * The library is platform independent (by invoking only those tests that are available on each individual platform).
 * @}
 */

/**
 * @namespace Ocean::Test::TestPlatform Namespace of the Platform Test library.<p>
 * The Namespace Ocean::Test::TestPlatform is used in the entire Ocean Platform Test Library.
 */

// Defines OCEAN_TEST_PLATFORM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_PLATFORM_EXPORT
		#define OCEAN_TEST_PLATFORM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_PLATFORM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_PLATFORM_EXPORT
#endif

/**
 * Tests all Platform libraries.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testplatform
 */
OCEAN_TEST_PLATFORM_EXPORT bool testPlatform(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests all Platform libraries.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testplatform
 */
OCEAN_TEST_PLATFORM_EXPORT void testPlatformAsynchron(const double testDuration, const std::string& testFunctions = std::string());
}

}

}

#endif // META_OCEAN_TEST_TESTPLATFORM_TEST_PLATFORM_H
