// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TEST_TESTIO_TESTIO_H
#define META_OCEAN_TEST_TESTIO_TESTIO_H

#include "ocean/test/Test.h"

#include "ocean/io/IO.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * @ingroup test
 * @defgroup testio Ocean Test IO Library
 * @{
 * The Ocean Test IO Library provides several function to test the performance and validation of the Ocean IO Library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestIO Namespace of the IO Test library.<p>
 * The Namespace Ocean::Test::TestIO is used in the entire Ocean IO Test Library.
 */

// Defines OCEAN_TEST_IO_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_IO_EXPORT
		#define OCEAN_TEST_IO_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_IO_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_IO_EXPORT
#endif

/**
 * Tests the entire io library.
 * @param testDuration Number of seconds for each test
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testio
 */
OCEAN_TEST_IO_EXPORT bool testIO(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTIO_H
