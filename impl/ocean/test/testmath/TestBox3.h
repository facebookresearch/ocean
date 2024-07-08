/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_BOX3_H
#define META_OCEAN_TEST_TESTMATH_TEST_BOX3_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/math/Box3.h"
#include "ocean/math/Line3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements box tests.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestBox3
{
	public:

		/**
		 * Tests the entire box functionalities.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Point test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testPoint(const double testDuration);

		/**
		 * Is inside test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testIsInside(const double testDuration);

		/**
		 * Addition test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testAddition(const double testDuration);

		/**
		 * Transformation test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testTransformation(const double testDuration);

		/**
		 * Line test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testLine(const double testDuration);

		/**
		 * Plane test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testPlane(const double testDuration);

		/**
		 * Space test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testSpace(const double testDuration);

		/**
		 * Has intersection test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testHasIntersection(const double testDuration);

		/**
		 * Tests the expand function.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool testExpand(const double testDuration);

	private:

		/**
		 * Validate has intersection test.
		 * @param testDuration Duration of each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool validateHasIntersection(const double testDuration);

		/**
		 * Returns whether a ray has an intersection with a given box or not.
		 * @param box Box to be tested
		 * @param line Line to be tested
		 * @return True, if so
		 * @tparam T The data type of the scalar to be used for testing, either 'float' or 'double'
		 */
		template <typename T>
		static bool hasIntersection(const BoxT3<T>& box, const LineT3<T>& line);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_BOX3_H
