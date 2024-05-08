/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_LIBYUV_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_LIBYUV_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

/**
 * @ingroup testcv
 *
 * @defgroup testcvlibyuv Ocean Test CV libyuv Library
 * @{
 * The Ocean Test CV libyuv Library provides several functions to test the performance of the 3rdparty libyuv library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestLibyuv Namespace of the Ocean CV libyuv test library.<p>
 * The Namespace Ocean::Test::TestCV:TestLibyuv is used in the entire Ocean CV libyuv Library.
 */

// Defines OCEAN_TEST_CV_LIBYUV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_LIBYUV_EXPORT
		#define OCEAN_TEST_CV_LIBYUV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_LIBYUV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_LIBYUV_EXPORT
#endif

/**
 * Tests the entire CV libyuv library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param skipValidation True, to skip the validation
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvlibyuv
 */
OCEAN_TEST_CV_LIBYUV_EXPORT void testCVLibyuv(const double testDuration, const bool skipValidation, const std::string& testFunctions = std::string());

/**
 * Tests the entire CV libyuv library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param skipValidation True, to skip the validation
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvlibyuv
 */
OCEAN_TEST_CV_LIBYUV_EXPORT void testCVLibyuvAsynchron(const double testDuration, const bool skipValidation, const std::string& testFunctions = std::string());


} // namespace TestLibyuv

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_LIBYUV_H
