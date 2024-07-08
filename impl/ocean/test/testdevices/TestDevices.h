/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTDEVICES_TEST_DEVICES_H
#define META_OCEAN_TEST_TESTDEVICES_TEST_DEVICES_H

#include "ocean/test/Test.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * @ingroup test
 * @defgroup testdevices Ocean Test Devices Library
 * @{
 * The Ocean Test Devices Library provides several functions to test the performance and validation of the Devices functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestDevices Namespace of the Devices Test library.<p>
 * The Namespace Ocean::Test::TestDevices is used in the entire Ocean Devices Test Library.
 */

// Defines OCEAN_TEST_DEVICES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_DEVICES_EXPORT
		#define OCEAN_TEST_DEVICES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_DEVICES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_DEVICES_EXPORT
#endif

/**
 * Tests the entire Devices library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testdevices
 */
OCEAN_TEST_DEVICES_EXPORT bool testDevices(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire Devices library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * This function is intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testdevices
 */
OCEAN_TEST_DEVICES_EXPORT void testDevicesAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTDEVICES_TEST_DEVICES_H
