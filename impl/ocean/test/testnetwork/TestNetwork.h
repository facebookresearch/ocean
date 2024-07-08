/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTNETWORK_TEST_NETWORK_H
#define META_OCEAN_TEST_TESTNETWORK_TEST_NETWORK_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

#include "ocean/network/Network.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

/**
 * @ingroup test
 * @defgroup testnetwork Ocean Test Network Library
 * @{
 * The Ocean Test Network Library provides several functions to test the performance and validation of the Ocean Network Library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestNetwork Namespace of the Network Test library.
 * The Namespace Ocean::Test::TestNetwork is used in the entire Ocean Network Test Library.
 */

// Defines OCEAN_TEST_NEWORK_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_NETWORK_EXPORT
		#define OCEAN_TEST_NETWORK_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_NETWORK_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_NETWORK_EXPORT
#endif

/**
 * Tests the entire network library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testnetwork
 */
OCEAN_TEST_NETWORK_EXPORT bool testNetwork(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire network library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testnetwork
 */
OCEAN_TEST_NETWORK_EXPORT void testNetworkAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTNETWORK_TEST_NETWORK_H
