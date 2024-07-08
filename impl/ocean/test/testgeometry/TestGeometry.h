/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TESTGEOMETRY_H
#define META_OCEAN_TEST_TESTGEOMETRY_TESTGEOMETRY_H

#include "ocean/test/Test.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * @ingroup test
 * @defgroup testgeometry Ocean Test Geometry Library
 * @{
 * The Ocean Test Geometry Library provides several function to test the performance and validation of geometry functions.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestGeometry Namespace of the Geometry Test library.<p>
 * The Namespace Ocean::Test::TestGeometry is used in the entire Ocean Geometry Test Library.
 */

// Defines OCEAN_TEST_GEOMETRY_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_GEOMETRY_EXPORT
		#define OCEAN_TEST_GEOMETRY_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_GEOMETRY_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_GEOMETRY_EXPORT
#endif

/**
 * Tests the entire Geometry library.
 * @param testDuration Number of seconds for each test
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testgeometry
 */
OCEAN_TEST_GEOMETRY_EXPORT bool testGeometry(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire Geometry library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testgeometry
 */
OCEAN_TEST_GEOMETRY_EXPORT void testGeometryAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TESTGEOMETRY_H
