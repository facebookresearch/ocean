/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_CV_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TEST_CV_UTILITIES_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the CVUtilities class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestCVUtilities
{
	public:

		/**
		 * Starts all tests of the CVUtilities class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector = TestSelector());

		/**
		 * Tests the definedPixelFormats() function.
		 * @return True, if succeeded
		 */
		static bool testDefinedPixelFormats();

		/**
		 * Tests the mirrorIndex() and mirrorOffset() helper functions.
		 * @return True, if succeeded
		 */
		static bool testMirrorIndex();

		/**
		 * Tests the isBorderZero() function.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsBorderZero(const double testDuration);

		/**
		 * Tests the isPaddingMemoryIdentical() function.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIsPaddingMemoryIdentical(const double testDuration);

		/**
		 * Tests the createCheckerboardImage() function.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCheckerboardImage(const double testDuration);

		/**
		 * Tests the randomizeFrame() / randomizedFrame() helpers.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomizedFrame(const double testDuration);

		/**
		 * Tests the randomizedBinaryMask() helper.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomizedBinaryMask(const double testDuration);

		/**
		 * Tests the copyPixel() helpers.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyPixel(const double testDuration);

		/**
		 * Stress test: random indices through the mirrorIndex() function and
		 * verifies the index always lands inside [0, elements - 1].
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMirrorIndexStress(const double testDuration);

		/**
		 * Stress test: randomized large frame, randomly randomized many times
		 * across all defined data types, ensures padding is preserved when
		 * skipPaddingArea is requested.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRandomizedFrameStress(const double testDuration);

		/**
		 * Stress test: copyPixel patch-style overload over random frame
		 * coordinates, validates against a manual byte-wise copy.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyPixelStress(const double testDuration);

		/**
		 * Stress test: createCheckerboardImage with extreme/edge sizes.
		 * @return True, if succeeded
		 */
		static bool testCheckerboardImageEdge();
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_CV_UTILITIES_H
