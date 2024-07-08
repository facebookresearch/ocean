/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	protected:

		/**
		 * Definition of individual verification results.
		 */
		enum VerificationResult : uint32_t
		{
			/// The verification failed.
			VR_FAILED = 0u,
			/// The verification did not meet precision constraints.
			VR_LOW_PRECISION,
			/// The verification succeeded.
			VR_SUCCEEDED,
		};

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
		 * @param optionalRandomGenerator Optional explicit random generator object to be used, nullptr to use a default object
		 * @return The verification result
		 * @tparam T The scalar data type of the camera object
		 */
		template <typename T>
		static VerificationResult verifyAnyCamera(const AnyCameraT<T>& anyCamera, RandomGenerator* optionalRandomGenerator = nullptr);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_ANY_CAMERA_H
