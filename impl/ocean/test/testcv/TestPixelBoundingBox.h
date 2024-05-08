/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_PIXEL_BOUNDING_BOX_H
#define META_OCEAN_TEST_TESTCV_TEST_PIXEL_BOUNDING_BOX_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a pixel bounding box test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestPixelBoundingBox
{
	public:

		/**
		 * Tests the entire pixel bounding box functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructors(const double testDuration);

		/**
		 * Tests the isInside function for points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPointIsInside(const double testDuration);

		/**
		 * Tests the isInside function for bounding boxes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBoxIsInside(const double testDuration);

		/**
		 * Tests the hasIntersection function for bounding boxes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBoxHasIntersection(const double testDuration);

		/**
		 * Tests the isIntersection function for bounding boxes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBoxIsTouching(const double testDuration);

		/**
		 * Tests the extended function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testExtended(const double testDuration);

		/**
		 * Tests the multiplication operator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMultiplication(const double testDuration);

		/**
		 * Tests the division operator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDivision(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_PIXEL_BOUNDING_BOX_H
