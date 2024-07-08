/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_OCTREE_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_OCTREE_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements homography tests.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestOctree
{
	public:

		/**
		 * Tests all octree functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the closestPoints() function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testClosestPoints(const double testDuration);

		/**
		 * Tests the intersectingLeafs() function for rays.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIntersectingLeafsForRays(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_OCTREE_H
