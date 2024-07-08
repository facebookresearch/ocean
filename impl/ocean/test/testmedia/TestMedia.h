/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_MEDIA_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_MEDIA_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

#include "ocean/media/Media.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * @ingroup test
 * @defgroup testmedia Ocean Test Media Library
 * @{
 * The Ocean Test Media Library implemenets/provides several tests for Ocean's individual media libraries.
 * The library is platform independent (by invoking only those tests that are available on each individual platform).
 * @}
 */

/**
 * @namespace Ocean::Test::TestMedia Namespace of the Media Test library.<p>
 * The Namespace Ocean::Test::TestMedia is used in the entire Ocean Media Test Library.
 */

// Defines OCEAN_TEST_MEDIA_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_MEDIA_EXPORT
		#define OCEAN_TEST_MEDIA_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_MEDIA_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_MEDIA_EXPORT
#endif

/**
 * Tests all Media libraries.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testmedia
 */
OCEAN_TEST_MEDIA_EXPORT bool testMedia(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests all Media libraries.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testmedia
 */
OCEAN_TEST_MEDIA_EXPORT void testMediaAsynchron(const double testDuration, const std::string& testFunctions = std::string());
}

}

}

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_MEDIA_H
