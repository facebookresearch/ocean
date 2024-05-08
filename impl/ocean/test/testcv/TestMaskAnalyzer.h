/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_MASK_ANALYZER_H
#define META_OCEAN_TEST_TESTCV_TEST_MASK_ANALYZER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame analyzer test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestMaskAnalyzer
{
	public:

		/**
		 * Tests all mask analyzer functions.
		 * @param width The width of a test mask in pixel, with range [1, infinity)
		 * @param height The height of a test mask in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range [0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the bounding box detection function.
		 * @param width The width of a test mask in pixel, with range [1, infinity)
		 * @param height The height of a test mask in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetectBoundingBox(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bounding box detection function widt given rough bounding box.
		 * @param width The width of a test mask in pixel, with range [1, infinity)
		 * @param height The height of a test mask in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetectBoundingBoxWithRoughGuess(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the opaque bounding box detection function.
		 * @param width The width of a test mask in pixel, with range [1, infinity)
		 * @param height The height of a test mask in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range [0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDetectOpaqueBoundingBox(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the hasValue() function.
		 * @param width The width of a test mask in pixel, with range [1, infinity)
		 * @param height The height of a test mask in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasValue(const unsigned int width, const unsigned int height, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_MASK_ANALYZER_H
