/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_BIN_PACKING_H
#define META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_BIN_PACKING_H

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
 * This class implements test for bin packing.
 * @ingroup testcvsegmentation
 */
class OCEAN_TEST_CV_SEGMENTATION_EXPORT TestBinPacking
{
	public:

		/**
		 * Tests all bin packing functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the bin packing function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBinPacking(const double testDuration);
};

} // namespace TestSegmentation

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTSEGMENTATION_TEST_BIN_PACKING_H
