// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_CV_EXTERNAL_H
#define META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_CV_EXTERNAL_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestExternal
{

/**
 * @ingroup testcv
 * @defgroup testcvexteranl Ocean Test CV External Library
 * @{
 * The Ocean Test CV External Library provides several functions to test the performance and validation of utility functions for external libraries.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestExternal Namespace of the CV External Test library.<p>
 * The Namespace Ocean::Test::TestCV::TestExternal is used in the entire Ocean CV External Test Library.
 */

// Defines OCEAN_TEST_CV_EXTERNAL_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_EXTERNAL_EXPORT
		#define OCEAN_TEST_CV_EXTERNAL_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_EXTERNAL_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_EXTERNAL_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_CV_EXTERNAL_H
