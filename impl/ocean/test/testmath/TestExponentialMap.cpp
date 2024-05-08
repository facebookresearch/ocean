/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestExponentialMap.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/ExponentialMap.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestExponentialMap::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   ExponentialMap test:   ---";
	Log::info() << " ";

	allSucceeded = testConstructors(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ExponentialMap test succeeded.";
	}
	else
	{
		Log::info() << "ExponentialMap test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestExponentialMap, Constructors)
{
	EXPECT_TRUE(TestExponentialMap::testConstructors(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestExponentialMap::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing the constructors:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Timestamp startTimestamp(true);

	do
	{
		{
			// angle-axis rotation

			const Rotation rotation = Random::rotation();

			const ExponentialMap exponentialMap(rotation);

			const Rotation resultRotation = exponentialMap.rotation();

			if (resultRotation == rotation)
			{
				++validIterations;
			}

			++iterations;
		}

		{
			// axis and angle

			const Vector3 axis = Random::vector3();
			ocean_assert(axis.isUnit());

			const Scalar angle = Random::scalar(0, Numeric::pi2());

			const ExponentialMap exponentialMap(axis, angle);

			const Rotation resultRotation = exponentialMap.rotation();

			if (resultRotation == Rotation(axis, angle))
			{
				++validIterations;
			}

			++iterations;
		}

		{
			// non-normalized axis

			const Vector3 rotationValue = Random::vector3(-10, 10);

			const ExponentialMap exponentialMap(rotationValue.x(), rotationValue.y(), rotationValue.z());

			const Scalar angle = rotationValue.length();

			Vector3 axis(rotationValue);
			if (Numeric::isNotEqualEps(angle))
			{
				axis /= angle;
			}

			const Rotation resultRotation = exponentialMap.rotation();

			if ((Numeric::isEqualEps(angle) && resultRotation == Rotation()) || (!Numeric::isEqualEps(angle) && resultRotation == Rotation(axis, angle)))
			{
				++validIterations;
			}

			++iterations;

			const Quaternion resultQuaternion = exponentialMap.quaternion();

			if ((Numeric::isEqualEps(angle) && resultQuaternion == Quaternion(true)) || (!Numeric::isEqualEps(angle) && resultQuaternion == Quaternion(axis, angle)))
			{
				++validIterations;
			}

			++iterations;
		}

		{
			// quaternion

			const Quaternion quaternion = Random::quaternion();

			const ExponentialMap exponentialMap(quaternion);

			const Quaternion resultQuaternion = exponentialMap.quaternion();

			if (resultQuaternion == quaternion)
			{
				++validIterations;
			}

			++iterations;
		}

		{
			// 3x3 rotation matrix

			const Quaternion quaternion = Random::quaternion();

			const ExponentialMap exponentialMap = ExponentialMap(SquareMatrix3(quaternion));

			const Quaternion resultQuaternion = exponentialMap.quaternion();

			if (resultQuaternion == quaternion)
			{
				++validIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	Log::info() << " ";

	return percent >= 0.95;
}

}

}

}
