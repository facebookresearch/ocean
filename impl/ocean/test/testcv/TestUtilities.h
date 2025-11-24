/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TEST_UTILITIES_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Computer Vision utilities test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestUtilities
{
	public:

		/**
		 * Tests the entire utilities class.
		 * @param testDuration Number of seconds for each test
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector = TestSelector());

		/**
		 * Tests the pixel copy function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the pixel elements
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testCopyPixel(const double testDuration);

		/**
		 * Tests the index pixel copy function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T, unsigned int tChannels>
		static bool testCopyPixelWithIndex(const double testDuration);

		/**
		 * Tests the position pixel copy function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T, unsigned int tChannels>
		static bool testCopyPixelWithPosition(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_UTILITIES_H
