/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_TEST_TESTIO_TESTMAPS_TEST_IO_MAPS_H
#define FACEBOOK_OCEAN_TEST_TESTIO_TESTMAPS_TEST_IO_MAPS_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestMaps
{

/**
 * @ingroup testio
 * @defgroup testiomaps Ocean Test IO Maps Library
 * @{
 * The Ocean Test IO Maps Library provides several functions to test the performance and validation of the maps functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestIO::TestMaps Namespace of the IO Maps Test library.<p>
 * The Namespace Ocean::Test::TestIO::TestMaps is used in the entire Ocean IO Maps Test Library.
 */

// Defines OCEAN_TEST_IO_MAPS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_IO_MAPS_EXPORT
		#define OCEAN_TEST_IO_MAPS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_IO_MAPS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_IO_MAPS_EXPORT
#endif

/**
 * Tests the entire IO Maps library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testiomaps
 */
OCEAN_TEST_IO_MAPS_EXPORT bool testIOMaps(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire IO maps library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testiomaps
 */
OCEAN_TEST_IO_MAPS_EXPORT void testIOMapsAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

}

#endif // FACEBOOK_OCEAN_TEST_TESTCV_TESTDETECTOR_TESTCVDETECTOR_H
