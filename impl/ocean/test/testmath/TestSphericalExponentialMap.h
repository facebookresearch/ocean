/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_SPHERICAL_EXPONENTIAL_MAP_H
#define META_OCEAN_TEST_TESTMATH_TEST_SPHERICAL_EXPONENTIAL_MAP_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/test/TestSelector.h"

#include "ocean/math/SphericalExponentialMap.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements a test for SphericalExponentialMap.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestSphericalExponentialMap
{
	public:

		/**
		 * Executes all tests.
		 * @param testDuration Number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the constructor taking a reference vector and an offset vector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructorFromVectors(const double testDuration);

	protected:

		/**
		 * Verifies the contract of the class, `offset == SphericalExponentialMap(reference, offset).rotation() * reference`.
		 * @param sphericalExponentialMap The object to be verified
		 * @param reference The reference vector which has been used to create the object, must have unit length
		 * @param offset The offset vector which has been used to create the object, must have unit length
		 * @return True, if so
		 */
		static bool verifyRotation(const SphericalExponentialMap& sphericalExponentialMap, const Vector3& reference, const Vector3& offset);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_SPHERICAL_EXPONENTIAL_MAP_H
