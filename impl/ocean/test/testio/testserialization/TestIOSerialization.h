/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_IO_SERIALIZATION_H
#define META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_IO_SERIALIZATION_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

/**
 * @ingroup testio
 * @defgroup testioserialization Ocean Test IO Serialization Library
 * @{
 * The Ocean Test IO Serialization Library provides several functions to test the performance and validation of the serialization functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestIO::TestSerialization Namespace of the IO Serialization Test library.<p>
 * The Namespace Ocean::Test::TestIO::TestSerialization is used in the entire Ocean IO Serialization Test Library.
 */

// Defines OCEAN_TEST_IO_SERIALIZATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_IO_SERIALIZATION_EXPORT
		#define OCEAN_TEST_IO_SERIALIZATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_IO_SERIALIZATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_IO_SERIALIZATION_EXPORT
#endif

/**
 * Tests the entire IO Serialization library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testioserialization
 */
OCEAN_TEST_IO_SERIALIZATION_EXPORT bool testIOSerialization(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire IO Serialization library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testioserialization
 */
OCEAN_TEST_IO_SERIALIZATION_EXPORT void testIOSerializationAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TESTSERIALIZATION_TEST_IO_SERIALIZATION_H
