/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_CREATOR_H
#define META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_CREATOR_H

#include "ocean/test/testcv/testsegmentation/TestCVSegmentation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

/**
 * This class implements a mask analyzer test.
 * @ingroup testcvsegmentation
 */
class OCEAN_TEST_CV_SEGMENTATION_EXPORT TestMaskCreator
{
	public:

		/**
		 * Tests all mask creator functions.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the join masks function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testJoinMasks(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_MASK_CREATOR_H
