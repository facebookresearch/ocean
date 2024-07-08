/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TESTMATH_H
#define META_OCEAN_TEST_TESTMATH_TESTMATH_H

#include "ocean/test/Test.h"

#include "ocean/math/Math.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * @ingroup test
 * @defgroup testmath Ocean Test Math Library
 * @{
 * The Ocean Test Math Library provides several functions to test the performance and validation of the Ocean Math Library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestMath Namespace of the Math Test library.<p>
 * The Namespace Ocean::Test::TestMath is used in the entire Ocean Math Test Library.
 */

// Defines OCEAN_TEST_MATH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_MATH_EXPORT
		#define OCEAN_TEST_MATH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_MATH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_MATH_EXPORT
#endif

/**
 * Tests the entire math library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testmath
 */
OCEAN_TEST_MATH_EXPORT bool testMath(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire math library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testmath
 */
OCEAN_TEST_MATH_EXPORT void testMathAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TESTMATH_H
