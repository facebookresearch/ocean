// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_PERCEPTION_H
#define META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_PERCEPTION_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/testmath/TestAnyCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

namespace TestPerception
{

/**
 * This class implements tests for the AnyCameraPerception class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestAnyCameraPerception : protected TestAnyCamera
{
	public:

		/**
		 * Tests all camera functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_PERCEPTION_H
