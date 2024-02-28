// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_H
#define META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements tests for the AnyCamera class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestAnyCamera
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
		 * @tparam T The scalar data type to use
		 */
		template <typename T>
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the principal point.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The scalar data type to use
		 */
		template <typename T>
		static bool testPrincipalPoint(const double testDuration);

	protected:

		/**
		 * Verifies an AnyCamera object.
		 * @param anyCamera The object to be verified, must be valid
		 * @param testDuration Number of seconds for the verification, with range (0, infinity)
		 * @param optionalRandomGenerator Optional explicit random generator object to be used, nullptr to use a default object
		 * @return True, if succeeded
		 * @tparam T The scalar data type of the camera object
		 */
		template <typename T>
		static bool verifyAnyCamera(const AnyCameraT<T>& anyCamera, const double testDuration, RandomGenerator* optionalRandomGenerator = nullptr);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_H
