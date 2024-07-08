/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SORTED_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SORTED_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameFilterSorted.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the sorted frame filter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterSorted : protected CV::FrameFilterSorted
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the histogram for integer values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHistogram(const double testDuration);

		/**
		 * Tests the sorted elements object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSortedElements(const double testDuration);

	protected:

		/**
		 * Tests a specified histogram.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam THistogram The type of the histogram to be tested
		 */
		template <typename THistogram>
		static bool testHistogram(const double testDuration);

		/**
		 * Tests the sorted elements for a specified data type.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the elements to be tested
		 */
		template <typename T>
		static bool testSortedElements(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SORTED_H
