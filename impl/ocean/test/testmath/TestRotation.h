// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H
#define META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a rotation test.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestRotation
{
	public:

		/**
		 * Executes all rotation tests.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the conversion from a rotation to a quaternion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionToQuaterion(const double testDuration);

		/**
		 * Tests the conversion from a rotation to a homogenous matrix.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionToHomogenousMatrix(const double testDuration);

		/**
		 * Tests the reference-offset constructor.
		 * @param testDuration Number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testReferenceOffsetConstructor(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_ROTATION_H
