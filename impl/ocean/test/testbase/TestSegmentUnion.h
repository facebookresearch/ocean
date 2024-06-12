/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SEGMENT_UNION_H
#define META_OCEAN_TEST_TESTBASE_TEST_SEGMENT_UNION_H

#include "ocean/test/testbase/TestBase.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implement a test for the SegmentUnion class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestSegmentUnion
{
	public:

		/**
		 * Invokes all test for the SegmentUnion class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the function determining the size of the union.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type defining the end points
		 */
		template <typename T>
		static bool testUnionSize(const double testDuration);

		/**
		 * Tests the function determining the intersection of the union with a segment.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type defining the end points
		 */
		template <typename T>
		static bool testIntersection(const double testDuration);

		/**
		 * Tests the function determining the maximal gap between segments.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type defining the end points
		 */
		template <typename T>
		static bool testMaximalGap(const double testDuration);

		/**
		 * Tests the bool cast operator.
		 * @return True, if succeeded
		 * @tparam T The data type defining the end points
		 */
		template <typename T>
		static bool testBoolCastOperator();
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SEGMENT_UNION_H
