// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_PERCEPTION_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_PERCEPTION_UTILITIES_H

#include "ocean/test/testcv/testexternal/TestCVExternal.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestExternal
{

/**
 * This class implements a test for the CV::PerceptionUtilities class.
 * @ingroup testexternal
 */
class OCEAN_TEST_CV_EXTERNAL_EXPORT TestPerceptionUtilities
{
	public:

		/**
		 * Runs all tests for PerceptionUtlities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the fromFisheyeCamera function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFromFisheyeCamera(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTEXTERNAL_TEST_PERCEPTION_UTILITIES_H
