/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SPHERE_3_H
#define META_OCEAN_TEST_TESTMATH_TEST_SPHERE_3_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for the 3D sphere.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSphere3
{
	public:

		/**
		 * Tests all sphere functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sphere-ray intersection function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testHasIntersection(const double testDuration);

		/**
		 * Tests the sphere-ray intersection function for a transformed sphere.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testHasIntersectionTransformed(const double testDuration);

		/**
		 * Tests the coordinate/vector conversion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testCoordinateVectorConversion(const double testDuration);

		/**
		 * Tests the shortest distance function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testShortestDistance(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SPHERE_3_H
